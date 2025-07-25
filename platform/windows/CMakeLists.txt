file(RELATIVE_PATH CURRENT_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(PLATFORM_DOC_CLASSES "EditorExportPlatformWindows")
set(PLATFORM_DOC_CLASS_PATH "${CURRENT_RELATIVE_PATH}/doc_classes")

foreach(DOC_CLASS ${PLATFORM_DOC_CLASSES})
	list(APPEND DOC_CLASS_PATHS "${DOC_CLASS},${PLATFORM_DOC_CLASS_PATH}")
endforeach()

set(DOC_CLASS_PATHS ${DOC_CLASS_PATHS} PARENT_SCOPE)

if(REX_PLATFORM STREQUAL "windows")
	set(TARGET_WIN_VERSION "0x0601" CACHE STRING "Targeted Windows version, >= 0x0601 (Windows 7)")
	set(WINDOWS_SUBSYSTEM "gui" CACHE STRING "Windows subsystem: gui, console")
	option(USE_MINGW "Use the Mingw compiler, even if MSVC is installed." OFF)
	option(USE_LLVM "Use the LLVM compiler" OFF)
	option(USE_STATIC_CPP "Link MinGW/MSVC C++ runtime libraries statically" ON)
	option(USE_ASAN "Use address sanitizer (ASAN)" OFF)
	option(USE_UBSAN "Use LLVM compiler undefined behavior sanitizer (UBSAN)" OFF)
	option(DEBUG_CRT "Compile with MSVC's debug CRT (/MDd)" OFF)
	option(INCREMENTAL_LINK "Use MSVC incremental linking. May increase or decrease build times." OFF)
	option(SILENCE_MSVC "Silence MSVC's cl/link stdout bloat, redirecting any errors to stderr." ON)
	set(ANGLE_LIBS "" CACHE STRING "Path to the ANGLE static libraries")
	option(USE_PIX "Enable PIX profiling support" OFF)
	set(PIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/pix" CACHE PATH "Path to PIX SDK")
	set(MESA_LIBS "${CMAKE_SOURCE_DIR}/thirdparty/mesa" CACHE PATH "Path to Mesa libraries")

	# To match other platforms
	set(STACK_SIZE 8388608)
	set(STACK_SIZE_SANITIZERS 31457280)

	if (MSVC)
		set(WINDOWS_CRASH_HANDLER ${CMAKE_CURRENT_SOURCE_DIR}/crash_handler_windows_seh.cpp)
	else()
		set(WINDOWS_CRASH_HANDLER ${CMAKE_CURRENT_SOURCE_DIR}/crash_handler_windows_signal.cpp)
	endif()

	add_executable(rex_engine
		${CMAKE_CURRENT_SOURCE_DIR}/godot_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/os_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/display_server_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/key_mapping_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/joypad_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tts_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/windows_terminal_logger.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/windows_utils.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/native_menu_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/gl_manager_windows_native.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/gl_manager_windows_angle.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/wgl_detect_version.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/rendering_context_driver_vulkan_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/drop_target_windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/godot_res.rc
		${WINDOWS_CRASH_HANDLER}
	)
	if (MSVC)
		set(DISABLED_WARNINGS
			/wd4100
			/wd4127
			/wd4201
			/wd4244
			/wd4245
			/wd4267
			/wd4305
			/wd4324
			/wd4514
			/wd4714
			/wd4820
		)
		if (WARNINGS STREQUAL "extra")
			target_compile_options(rex_engine PUBLIC
				/W4
				${DISABLED_WARNINGS}
			)
		elseif(WARINGS STREQUAL "all")
			target_compile_options(rex_engine PUBLIC
				/W3
				/w34458
				${DISABLED_WARNINGS}
			)
		elseif(WARINGS STREQUAL "moderate")
			target_compile_options(rex_engine PUBLIC
				/W2
				${DISABLED_WARNINGS}
			)
		else()
			target_compile_options(rex_engine PUBLIC
				/w
				/wd4267
			)
		endif()

		if (BUILD_TARGET STREQUAL "template_release")
			target_link_options(rex_engine PUBLIC /ENTRY:mainCRTStartup)
		endif()

		if (WINDOWS_SUBSYSTEM STREQUAL "gui")
			target_link_options(rex_engine PRIVATE /SUBSYSTEM:WINDOWS)
		else()
			target_link_options(rex_engine PRIVATE /SUBSYSTEM:CONSOLE)
			target_compile_definitions(rex_engine PUBLIC WINDOWS_SUBSYSTEM_CONSOLE)
		endif()

		if (USE_LLVM)
			find_program(CMAKE_C_COMPILER clang-cl PATHS ENV PATH)
			find_program(CMAKE_CXX_COMPILER clang-cl PATHS ENV PATH)
			find_program(CMAKE_LINKER lld-link PATHS ENV PATH)
			find_program(CMAKE_AR llvm-lib PATHS ENV PATH)
			if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER OR NOT CMAKE_LINKER OR NOT CMAKE_AR)
				message(FATAL_ERROR "LLVM tools (clang-cl, lld-link, llvm-lib) not found")
			endif()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D -I")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D -I")
			target_compile_definitions(rex_engine PUBLIC
				R128_STDC_ONLY
				alloca=_alloca
			)
			set(CMAKE_EXECUTABLE_SUFFIX ".llvm${CMAKE_EXECUTABLE_SUFFIX}")
		endif()

		if (SILENCE_MSVC)
			# TODO
		endif()
		if (DEBUG_CRT)
			target_compile_options(rex_engine PUBLIC /MDd)
		else()
			if (USE_STATIC_CPP)
				target_compile_options(rex_engine PUBLIC /MT)
			else()
				target_compile_options(rex_engine PUBLIC /MD)
			endif()
		endif()

		if (NOT INCREMENTAL_LINK)
			target_link_options(rex_engine PUBLIC /INCREMENTAL:NO)
		endif()

		target_compile_options(rex_engine PUBLIC
			/fp:strict
			/Gd
			/GR
			/nologo
			/utf-8
			/bigobj
		)

		if (ACCESSKIT)
			# Convert TARGET_WIN_VERSION to integer for comparison
			string(SUBSTRING "${TARGET_WIN_VERSION}" 2 -1 HEX_VERSION)
			math(EXPR DEC_VERSION "0${HEX_VERSION}" OUTPUT_FORMAT DECIMAL)
			if(DEC_VERSION LESS 602)  # 0x0602 = 602 in decimal
				message(STATUS "AccessKit enabled, targeted Windows version changed to Windows 8 (0x0602).")
				set(TARGET_WIN_VERSION "0x0602" CACHE STRING "Target Windows version" FORCE)
			endif()
		endif()

		target_compile_definitions(rex_engine PUBLIC
			TYPED_METHOD_BIND
			WIN32
			NOMINMAX
		)
		if (ARCH STREQUAL "x86_64")
			target_compile_definitions(rex_engine PUBLIC
				_WIN64
			)
		endif()

		set(PREBUILT_LIB_EXTRA_SUFFIX "")
		if (USE_ASAN)
			set(CMAKE_EXECUTABLE_SUFFIX ".san${CMAKE_EXECUTABLE_SUFFIX}")
			set(PREBUILT_LIB_EXTRA_SUFFIX ".san")
			target_compile_options(rex_engine PUBLIC /fsanitize=address)
			target_compile_definitions(rex_engine PUBLIC SANITIZERS_ENABLED)
			target_link_options(rex_engine PUBLIC /INFERASANLIBS)
		endif()

		if (USE_LLVM)
			list(APPEND SYSTEM_LIBRARIES
				clang_rt.builtins-${ARCH}
			)
		endif()

		if (ACCESSKIT)
			if (NOT ACCESSKIT_SDK_PATH STREQUAL "")
				target_include_directories(rex_engine PRIVATE "${ACCESSKIT_SDK_PATH}/include")
				if(ARCH STREQUAL "arm64")
					set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/arm64/msvc/static")
				elseif(ARCH STREQUAL "x86_64")
					set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/x86_64/msvc/static")
				elseif(ARCH STREQUAL "x86_32")
					set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/x86/msvc/static")
				else()
					message(FATAL_ERROR "Unsupported architecture for AccessKit: ${ARCH}")
				endif()
				find_library(ACCESSKIT_LIB accesskit PATHS "${ACCESSKIT_LIB_PATH}" NO_DEFAULT_PATH)
				if(NOT ACCESSKIT_LIB)
					message(FATAL_ERROR "accesskit library not found in ${ACCESSKIT_LIB_PATH}")
				endif()
				list(APPEND SYSTEM_LIBRARIES
					${ACCESSKIT_LIB}
					uiautomationcore
					runtimeobject
					propsys
					oleaut32
					user32
					userenv
					ntdll
				)
			else()
				target_compile_definitions(rex_engine PUBLIC ACCESSKIT_DYNAMIC)
			endif()
			target_compile_definitions(rex_engine PUBLIC ACCESSKIT_ENABLED)
		endif()

		if (LTO STREQUAL "auto")  # No LTO by default for MSVC, doesn't help.
			set(LTO "none")
		endif()
		if (NOT LTO STREQUAL "none")
			if (LTO STREQUAL "thin")
				if (NOT USE_LLVM)
					MESSAGE(FATAL_ERROR "ThinLTO is only compatible with LLVM, use `use_llvm=yes` or `lto=full`.")
				endif()
				target_compile_options(rex_engine PUBLIC -flto=thin)
			elseif(USE_LLVM)
				target_compile_options(rex_engine PUBLIC -flto)
			else()
				target_compile_options(rex_engine PUBLIC /GL)
			endif()
			if (PROGRESS)
				target_link_options(rex_engine PUBLIC /LTCG:STATUS)
			else()
				target_link_options(rex_engine PUBLIC /LTCG)
			endif()
		endif()

		if (PROGRESS)
			target_link_options(rex_engine PUBLIC /LTCG:STATUS)
		else()
			target_link_options(rex_engine PUBLIC /LTCG)
		endif()

		if (USE_ASAN)
			target_link_options(rex_engine PUBLIC /STACK:${STACK_SIZE_SANITIZERS})
		else()
			target_link_options(rex_engine PUBLIC /STACK:${STACK_SIZE})
		endif()

		set(SYSTEM_LIBRARIES
			advapi32
		)

		#MINGW
	else()
		# TODO: Various checks Scons performs

		if (REX_DEV_BUILD)
			target_compile_options(rex_engine PUBLIC -Wa,-mbig-obj)
		endif()

		if (WINDOWS_SUBSYSTEM STREQUAL "gui")
			target_link_options(rex_engine PRIVATE -Wl,--subsystem,windows)
		else()
			target_link_options(rex_engine PRIVATE -Wl,--subsystem,console)
			target_compile_definitions(rex_engine PUBLIC WINDOWS_SUBSYSTEM_CONSOLE)
		endif()

		if (USE_STATIC_CPP)
			target_link_options(rex_engine PUBLIC -static)
			if (ARCH STREQUAL "x86_32")
				target_link_options(rex_engine PUBLIC
					-static-libgcc
					-static-libstdc++
				)
			endif()
		endif()

		target_compile_options(rex_engine PUBLIC -ffp-contract=off)

		if (USE_LLVM)
			find_program(CMAKE_C_COMPILER clang PATHS ENV PATH)
			find_program(CMAKE_CXX_COMPILER clang++ PATHS ENV PATH)
			find_program(CMAKE_AR ar PATHS ENV PATH)
			find_program(CMAKE_RANLIB ranlib PATHS ENV PATH)
			if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER OR NOT CMAKE_AR OR NOT CMAKE_RANLIB)
				message(FATAL_ERROR "LLVM tools (clang, clang++, ar, ranlib) not found")
			endif()
			set(CMAKE_EXECUTABLE_SUFFIX ".llvm${CMAKE_EXECUTABLE_SUFFIX}")
		else()
			find_program(CMAKE_C_COMPILER gcc PATHS ENV PATH)
			find_program(CMAKE_CXX_COMPILER g++ PATHS ENV PATH)
			find_program(CMAKE_AR gcc-ar PATHS ENV PATH)
			find_program(CMAKE_RANLIB gcc-ranlib PATHS ENV PATH)
			if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER OR NOT CMAKE_AR OR NOT CMAKE_RANLIB)
				message(FATAL_ERROR "GCC tools (gcc, g++, gcc-ar, gcc-ranlib) not found")
			endif()
			target_compile_options(rex_engine PUBLIC -mwindows)
		endif()

		if (ARCH STREQUAL "x86_32")
			set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --target=pe-i386")
		elseif(ARCH STREQUAL "x86_64")
			set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --target=pe-x86-64")
		elseif(ARCH STREQUAL "arm32")
			set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --target=armv7-w64-mingw32")
		elseif(ARCH STREQUAL "arm64")
			set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --target=aarch64-w64-mingw32")
		endif()

		set(PREBUILT_LIB_EXTRA_SUFFIX "")
		if (USE_ASAN OR USE_UBSAN)
			if (USE_LLVM)
				message(FATAL_ERROR "GCC does not support sanitizers on Windows.")
			endif()
			if (NOT ARCH MATCHES "x86_32|x86_64")
				message(FATAL_ERROR "Sanitizers are only supported for x86_32 and x86_64.")
			endif()

			set(CMAKE_EXECUTABLE_SUFFIX ".san${CMAKE_EXECUTABLE_SUFFIX}")
			set(PREBUILT_LIB_EXTRA_SUFFIX ".san")

			target_compile_definitions(rex_engine PUBLIC SANITIZERS_ENABLED)
			if (USE_ASAN)
				target_compile_options(rex_engine PUBLIC -fsanitize=address)
				target_link_options(rex_engine PUBLIC -fsanitize=address)
			endif()
			if (USE_UBSAN)
				target_compile_options(rex_engine PUBLIC
					-fsanitize=undefined
					-fno-sanitize=vptr
				)
				target_link_options(rex_engine PUBLIC
					-fsanitize=undefined
					-fno-sanitize=vptr
				)
			endif()
		endif()

		if (LTO STREQUAL "auto")  # No LTO by default for MSVC, doesn't help.
			if (USE_LLVM)
				set(LTO "thin")
			else()
				set(LTO "full")
			endif()
		endif()

		if (NOT LTO STREQUAL "none")
			if (LTO STREQUAL "thin")
				if (NOT USE_LLVM)
					MESSAGE(FATAL_ERROR "ThinLTO is only compatible with LLVM, use `use_llvm=yes` or `lto=full`.")
				endif()
				target_compile_options(rex_engine PUBLIC -flto=thin)
				target_link_options(rex_engine PUBLIC -flto=thin)
			elseif(NOT USE_LLVM)
				target_compile_options(rex_engine PUBLIC -flto)
				if (NUM_JOBS GREATER 1)
					target_link_options(rex_engine PUBLIC -flto=${NUM_JOBS})
				endif()
			else()
				target_compile_options(rex_engine PUBLIC -flto)
				target_link_options(rex_engine PUBLIC -flto)
			endif()
			if (NOT USE_LLVM)
				target_compile_options(rex_engine PUBLIC
					-fno-use-linker-plugin
					-fwhole-program
				)
				target_link_options(rex_engine PUBLIC
					-fno-use-linker-plugin
					-fwhole-program
				)
			endif()
		endif()

		if (USE_ASAN)
			target_link_options(rex_engine PUBLIC -Wl,--stack,${STACK_SIZE_SANITIZERS})
		else()
			target_link_options(rex_engine PUBLIC -Wl,--stack,${STACK_SIZE})
		endif()

		set(SYSTEM_LIBRARIES
			mingw32
			d3d9
			uuid
			ksuser
		)

		if (ACCESSKIT)
			if (NOT ACCESSKIT_SDK_PATH STREQUAL "")
				target_include_directories(rex_engine PRIVATE "${ACCESSKIT_SDK_PATH}/include")
				if (USE_LLVM)
					if (ARCH STREQUAL "arm64")
						set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/arm64/mingw-llvm/static")
					elseif(ARCH STREQUAL "x86_64")
						set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/x86_64/mingw-llvm/static")
					elseif(ARCH STREQUAL "x86_32")
						set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/x86/mingw-llvm/static")
					else()
						message(FATAL_ERROR "Unsupported architecture for AccessKit: ${ARCH}")
					endif()
				else()
					if(ARCH STREQUAL "x86_64")
						set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/x86_64/mingw/static")
					elseif(ARCH STREQUAL "x86_32")
						set(ACCESSKIT_LIB_PATH "${ACCESSKIT_SDK_PATH}/lib/windows/x86/mingw/static")
					else()
						message(FATAL_ERROR "Unsupported architecture for AccessKit: ${ARCH}")
					endif()
				endif()
				find_library(ACCESSKIT_LIB accesskit PATHS "${ACCESSKIT_LIB_PATH}" NO_DEFAULT_PATH)
				if (NOT ACCESSKIT_LIB)
					message(FATAL_ERROR "accesskit library not found in ${ACCESSKIT_LIB_PATH}")
				endif()
				list(APPEND SYSTEM_LIBRARIES
					${ACCESSKIT_LIB}
					uiautomationcore.${ARCH}
					runtimeobject
					propsys
					oleaut32
					user32
					userenv
					ntdll
				)
			else()
				target_compile_definitions(rex_engine PUBLIC ACCESSKIT_DYNAMIC)
			endif()
			target_compile_definitions(rex_engine PUBLIC ACCESSKIT_ENABLED)
		endif()

		target_compile_definitions(rex_engine PUBLIC
			MINGW_ENABLED
			MINGW_HAS_SECURE_API=1
		)
	endif()

	target_compile_definitions(rex_engine PUBLIC
		WINDOWS_ENABLED
		WASAPI_ENABLED
		WINMIDI_ENABLED
		WINVER=${TARGET_WIN_VERSION}
		_WIN32_WINNT=${TARGET_WIN_VERSION}
	)
	target_include_directories(rex_engine PUBLIC
		${CMAKE_SOURCE_DIR}/platform/windows/
	)
	list(APPEND SYSTEM_LIBRARIES
		winmm
		dsound
		kernel32
		ole32
		oleaut32
		sapi
		user32
		gdi32
		IPHLPAPI
		Shlwapi
		wsock32
		Ws2_32
		shell32
		dinput8
		dxguid
		imm32
		bcrypt
		Crypt32
		Avrt
		dwmapi
		dwrite
		wbemuuid
		ntdll
	)

	if (DEBUG_FEATURES)
		list(APPEND SYSTEM_LIBRARIES
			psapi
			dbghelp
		)
	endif()

	if (WINDOWS_SUBSYSTEM STREQUAL "gui")
		add_executable(rex_console
			${CMAKE_CURRENT_SOURCE_DIR}/console_wrapper_windows.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/godot_res_wrap.rc
		)
		clone_library(rex_engine rex_console)
		disable_warnings_for_target(rex_console)

		target_link_libraries(rex_console PRIVATE ${SYSTEM_LIBRARIES})

		if (MSVC)
			target_link_options(rex_console PRIVATE
				/SUBSYSTEM:CONSOLE
				version.lib
			)
		else()
			target_link_options(rex_console PRIVATE -Wl,--subsystem,console)
			target_link_libraries(rex_console PRIVATE version)
		endif()
		set(OUTPUT_NAME "rex.${REX_PLATFORM}")
		if(EDITOR_BUILD)
			set(OUTPUT_NAME "${OUTPUT_NAME}.editor")
		else()
			set(OUTPUT_NAME "${OUTPUT_NAME}.opt")
		endif()
		if(REX_BUILD_TYPE MATCHES "debug")
			set(OUTPUT_NAME "${OUTPUT_NAME}.debug")
		endif()
		set_target_properties(rex_console PROPERTIES
			OUTPUT_NAME "${OUTPUT_NAME}.console"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
		)
		add_dependencies(rex_console rex_engine)
	endif()

	if (VULKAN)
		target_compile_definitions(rex_engine PUBLIC
			VULKAN_ENABLED
			RD_ENABLED
		)
		if (NOT USE_VOLK)
			list(APPEND SYSTEM_LIBRARIES vulkan)
		endif()
	endif()

	if (D3D12)
		if (MSVC)
			list(APPEND SYSTEM_LIBRARIES dxgi version)
		else()
			list(APPEND SYSTEM_LIBRARIES dxgi dxguid version)
		endif()
		target_compile_definitions(rex_engine PUBLIC
			D3D12_ENABLED
			RD_ENABLED
		)
		if (USE_PIX)
			if(ARCH MATCHES "x86_64|arm64" AND EXISTS "${PIX_PATH}")
				string(REPLACE "x86_" "x" PIX_ARCH_SUBDIR ${ARCH})
				set(PIX_LIB_PATH "${PIX_PATH}/bin/${PIX_ARCH_SUBDIR}")
				find_library(PIX_LIB WinPixEventRuntime PATHS "${PIX_LIB_PATH}" NO_DEFAULT_PATH)
				if(NOT PIX_LIB)
					message(WARNING "WinPixEventRuntime not found in ${PIX_LIB_PATH}; disabling PIX")
					set(USE_PIX OFF CACHE BOOL "Enable PIX profiling support" FORCE)
				else()
					list(APPEND SYSTEM_LIBRARIES ${PIX_LIB})
				endif()
			else()
				message(WARNING "PIX not supported for arch ${ARCH} or PIX_PATH ${PIX_PATH} does not exist; disabling PIX")
				set(USE_PIX OFF CACHE BOOL "Enable PIX profiling support" FORCE)
			endif()
		endif()
		if (MSVC)
			if (EXISTS "${MESA_LIBS}-${ARCH}-msvc/bin")
				set(MESA_LIB_PATH "${MESA_LIBS}-${ARCH}-msvc/bin")
			else()
				set(MESA_LIB_PATH "${MESA_LIBS}/bin")
			endif()
			find_library(NIR_LIB libNIR.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX}
				PATHS "${MESA_LIB_PATH}" NO_DEFAULT_PATH)
			if (NOT NIR_LIB)
				message(FATAL_ERROR "libNIR.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX} not found in ${MESA_LIB_PATH}")
			endif()
			list(APPEND	SYSTEM_LIBRARIES ${NIR_LIB})
		else()
			if (USE_LLVM AND EXISTS "${MESA_LIBS}-${ARCH}-llvm/bin")
				set(MESA_LIB_PATH "${MESA_LIBS}-${ARCH}-llvm/bin")
			elseif (NOT USE_LLVM AND EXISTS "${MESA_LIBS}-${ARCH}-gcc/bin")
				set(MESA_LIB_PATH "${MESA_LIBS}-${ARCH}-gcc/bin")
			else()
				set(MESA_LIB_PATH "${MESA_LIBS}/bin")
			endif()
			find_library(NIR_LIB libNIR.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX}
				PATHS "${MESA_LIB_PATH}" NO_DEFAULT_PATH)
			if (NOT NIR_LIB)
				message(FATAL_ERROR "libNIR.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX} not found in ${MESA_LIB_PATH}")
			endif()
			list(APPEND	SYSTEM_LIBRARIES ${NIR_LIB})
		endif()
	endif()

	if (OPENGL3)
		target_compile_definitions(rex_engine PUBLIC GLES3_ENABLED)
		if (NOT ANGLE_LIBS STREQUAL "")
			target_compile_definitions(rex_engine PUBLIC EGL_STATIC)
			find_library(ANGLE_LIB NAMES libANGLE.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX} libANGLE
				PATHS ${ANGLE_LIBS}
				NO_DEFAULT_PATH)
			find_library(EGL_LIB NAMES libEGL.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX} libEGL
				PATHS ${ANGLE_LIBS}
				NO_DEFAULT_PATH)
			find_library(GLES_LIB NAMES libGLES.windows.${ARCH}${PREBUILT_LIB_EXTRA_SUFFIX} libGLES
				PATHS ${ANGLE_LIBS}
				NO_DEFAULT_PATH)
			if(NOT ANGLE_LIB OR NOT EGL_LIB OR NOT GLES_LIB)
				message(FATAL_ERROR "ANGLE libraries not found in ${ANGLE_LIBS}")
			endif()
			list(APPEND SYSTEM_LIBRARIES ${ANGLE_LIB} ${EGL_LIB} ${GLES_LIB} dxgi d3d9 d3d11)
		endif()
		target_include_directories(rex_engine PUBLIC
			${CMAKE_SOURCE_DIR}/thirdparty/angle/include/
		)
	endif()

	set(SYSTEM_LIBRARIES ${SYSTEM_LIBRARIES} PARENT_SCOPE)
endif()
