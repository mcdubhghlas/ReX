file(RELATIVE_PATH CURRENT_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(PLATFORM_DOC_CLASSES "EditorExportPlatformMacOS")
set(PLATFORM_DOC_CLASS_PATH "${CURRENT_RELATIVE_PATH}/doc_classes")

foreach(DOC_CLASS ${PLATFORM_DOC_CLASSES})
	list(APPEND DOC_CLASS_PATHS "${DOC_CLASS},${PLATFORM_DOC_CLASS_PATH}")
endforeach()

set(DOC_CLASS_PATHS ${DOC_CLASS_PATHS} PARENT_SCOPE)

if(REX_PLATFORM STREQUAL "macos")
	set(OSXCROSS_SDK "darwin16" CACHE STRING "OSXCross SDK version")
	set(MACOS_SDK_PATH "" CACHE STRING "Path to the macOS SDK")
	set(VULKAN_SDK_PATH "" CACHE STRING "Path to the Vulkan SDK")
	set(MACPORTS_CLANG "no" CACHE STRING "Build using Clang from MacPorts: no, 5.0, devel")
	option(USE_UBSAN "Use LLVM/GCC compiler undefined behavior sanitizer (UBSAN)" OFF)
	option(USE_ASAN "Use LLVM/GCC compiler address sanitizer (ASAN)" OFF)
	option(USE_TSAN "Use LLVM/GCC compiler thread sanitizer (TSAN)" OFF)
	option(USE_COVERAGE "Use instrumentation codes in the binary (e.g. for code coverage)" OFF)
	set(ANGLE_LIBS "" CACHE STRING "Path to the ANGLE static libraries")
	set(BUNDLE_SIGN_IDENTITY "-" CACHE STRING "The 'Full Name', 'Common Name' or SHA-1 hash of the signing identity used
	to sign editor .app bundle.")
	option(GENERATE_BUNDLE "Generate an APP bundle after building iOS/macOS binaries" OFF)

	if(DEFINED ENV{OSXCROSS_ROOT})
		set(OSXCROSS_ENABLED ON CACHE BOOL "Enable OS X cross-compilation support")
	endif()

	set(STACK_SIZE 0x800000)   #8388608
	set(STACK_SIZE_SANITIZERS 0x1E00000)  #31457280

	add_executable(rex_engine
		${CMAKE_CURRENT_SOURCE_DIR}/os_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_application.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_application_delegate.mm
		${CMAKE_CURRENT_SOURCE_DIR}/crash_handler_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/macos_terminal_logger.mm
		${CMAKE_CURRENT_SOURCE_DIR}/display_server_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_button_view.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_content_view.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_status_item.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_window_delegate.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_window.mm
		${CMAKE_CURRENT_SOURCE_DIR}/key_mapping_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_main_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_menu_delegate.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_menu_item.mm
		${CMAKE_CURRENT_SOURCE_DIR}/godot_open_save_delegate.mm
		${CMAKE_CURRENT_SOURCE_DIR}/native_menu_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/dir_access_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/tts_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/rendering_context_driver_vulkan_macos.mm
		${CMAKE_CURRENT_SOURCE_DIR}/gl_manager_macos_angle.mm
		${CMAKE_CURRENT_SOURCE_DIR}/gl_manager_macos_legacy.mm
	)

	if (ARCH STREQUAL "arm64")
		message(STATUS "Building for macOS 11.0+.")
		target_compile_options(rex_engine PUBLIC
			-arch arm64
			-mmacosx-version-min=11.0
		)
		target_link_options(rex_engine PUBLIC
			-arch arm64
			-mmacosx-version-min=11.0
		)
	elseif (ARCH STREQUAL "x86_64")
		message(STATUS "Building for macOS 10.13+.")
		target_compile_options(rex_engine PUBLIC
			-arch x86_64
			-mmacosx-version-min=10.13
		)
		target_link_options(rex_engine PUBLIC
			-arch x86_64
			-mmacosx-version-min=10.13
		)
	endif()

	target_compile_options(rex_engine PUBLIC
		-ffp-contract=off
		-fobjc-arc
	)

	if (REX_DEV_BUILD)
		target_link_options(rex_engine PUBLIC
			-Xlinker
			-no_deduplicate
		)
	endif()

	set(CCACHE_PATH "")
	if (DEFINED ENV{CCACHE})
		set(CCACHE_PATH "$ENV{CCACHE} ")
	endif()

	if (OSXCROSS_ENABLED)
		set(ROOT "")
		if (DEFINED ENV{OSXCROSS_ROOT})
			set(ROOT "$ENV{OSXCROSS_ROOT}")
			if (ARCH STREQUAL "arm64")
				set(BUILDCMD "${ROOT}/target/bin/arm64-apple-${OSXCROSS_SDK}-")
			else()
				set(BUILDCMD "${ROOT}/target/bin/x86_64-apple-${OSXCROSS_SDK}-")
			endif()
			find_program(CMAKE_C_COMPILER ${CCACHE_PATH}${BUILDCMD}cc PATHS ENV PATH)
			find_program(CMAKE_CXX_COMPILER ${CCACHE_PATH}${BUILDCMD}c++ PATHS ENV PATH)
			find_program(CMAKE_AR ${BUILDCMD}ar PATHS ENV PATH)
			find_program(CMAKE_RANLIB ${BUILDCMD}ranlib PATHS ENV PATH)
			find_program(CMAKE_ASM_COMPILER ${BUILDCMD}as PATHS ENV PATH)
			if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER OR NOT CMAKE_AR OR NOT CMAKE_RANLIB OR NOT CMAKE_ASM_COMPILER)
				message(FATAL_ERROR "Build tools (cc, c++, ar, ranlib, as) not found")
			endif()
		endif()
	else()
		if (NOT MACPORTS_CLANG STREQUAL "no")
			set(MPPREFIX "/opt/local")
			if (DEFINED MACPORTS_PREFIX)
				set(MPPREFIX ${MACPORTS_PREFIX})
			endif()
			set(MPCLANGVER ${MACPORTS_CLANG})
			find_program(CMAKE_C_COMPILER ${MPPREFIX}/libexec/llvm-${MPCLANGVER}/bin/clang PATHS ENV PATH)
			find_program(CMAKE_CXX_COMPILER ${MPPREFIX}/libexec/llvm-${MPCLANGVER}/bin/clang++ PATHS ENV PATH)
			find_program(CMAKE_AR ${MPPREFIX}/libexec/llvm-${MPCLANGVER}/bin/llvm-ar PATHS ENV PATH)
			find_program(CMAKE_RANLIB ${MPPREFIX}/libexec/llvm-${MPCLANGVER}/bin/llvm-ranlib PATHS ENV PATH)
			find_program(CMAKE_ASM_COMPILER ${MPPREFIX}/libexec/llvm-${MPCLANGVER}/bin/llvm-as PATHS ENV PATH)
			if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER OR NOT CMAKE_AR OR NOT CMAKE_RANLIB OR NOT CMAKE_ASM_COMPILER)
				message(FATAL_ERROR "Build tools (clang, clang++, llvm-ar, llvm-ranlib, llvm-as) not found")
			endif()
		else()
			find_program(CMAKE_C_COMPILER ${CCACHE_PATH}clang PATHS ENV PATH)
			find_program(CMAKE_CXX_COMPILER ${CCACHE_PATH}clang++ PATHS ENV PATH)
			if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
				message(FATAL_ERROR "Build tools (clang, clang++) not found")
			endif()
		endif()
	endif()


	if (LTO STREQUAL "auto")  # LTO benefits for macOS (size, performance) haven't been clearly established yet.
		set(LTO "none")
	endif()
	if (NOT LTO STREQUAL "none")
		if (LTO STREQUAL "thin")
			target_compile_options(rex_engine PUBLIC -flto=thin)
			target_link_options(rex_engine PUBLIC -flto=thin)
		else()
			target_compile_options(rex_engine PUBLIC -flto)
			target_link_options(rex_engine PUBLIC -flto)
		endif()
	endif()

	if (USE_UBSAN OR USE_ASAN OR USE_TSAN)
		set(CMAKE_EXECUTABLE_SUFFIX ".san${CMAKE_EXECUTABLE_SUFFIX}")
		if (USE_UBSAN)
			target_compile_options(rex_engine PUBLIC
				-fsanitize=undefined,shift,shift-exponent,integer-divide-by-zero,unreachable,vla-bound,null,return,signed-integer-overflow,bounds,float-divide-by-zero,float-cast-overflow,nonnull-attribute,returns-nonnull-attribute,bool,enum,vptr,pointer-overflow,builtin
			)
			target_link_options(rex_engine PUBLIC -fsanitize=undefined)
			target_compile_options(rex_engine PUBLIC
				-fsanitize=nullability-return,nullability-arg,function,nullability-assign
			)
		endif()
		if (USE_ASAN)
			target_compile_options(rex_engine PUBLIC
				-fsanitize=address,pointer-subtract,pointer-compare
			)
			target_link_options(rex_engine PUBLIC -fsanitize=address)
		endif()
		if (USE_TSAN)
			target_compile_options(rex_engine PUBLIC -fsanitize=thread)
			target_link_options(rex_engine PUBLIC -fsanitize=thread)
		endif()
		target_link_options(rex_engine PUBLIC -Wl,-stack_size,${STACK_SIZE_SANITIZERS})
	else()
		target_link_options(rex_engine PUBLIC -Wl,-stack_size,${STACK_SIZE})
	endif()

	if (USE_COVERAGE)
		target_compile_options(rex_engine PUBLIC
			-ftest-coverage
			-fprofile-arcs
		)
		target_link_options(rex_engine PUBLIC
			-ftest-coverage
			-fprofile-arcs
		)
	endif()

	if (ACCESSKIT)
		if (NOT ACCESSKIT_SDK_PATH STREQUAL "")
			target_include_directories(rex_engine PRIVATE "${ACCESSKIT_SDK_PATH}/include")
			if (ARCH STREQUAL "arm64" OR ARCH STREQUAL "universal")
				target_link_options(rex_engine PUBLIC
					-L${ACCESSKIT_SDK_PATH}/lib/macos/arm64/static/
				)
			endif()
			if (ARCH STREQUAL "x86_64" OR ARCH STREQUAL "universal")
				target_link_options(rex_engine PUBLIC
					-L${ACCESSKIT_SDK_PATH}/lib/macos/x86_64/static/
				)
			endif()
			target_link_options(rex_engine PUBLIC -laccesskit)
		else()
			target_compile_definitions(rex_engine PUBLIC ACCESSKIT_DYNAMIC)
		endif()
		target_compile_definitions(rex_engine PUBLIC ACCESSKIT_ENABLED)
	endif()

	if (BUILTIN_LIBTHEORA AND ARCH STREQUAL "x86_64")
		set(X86_LIBTHEORA_OPT_GCC ON PARENT_SCOPE)
	endif()

	target_include_directories(rex_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
	target_compile_definitions(rex_engine PUBLIC
		MACOS_ENABLED
		UNIX_ENABLED
		COREAUDIO_ENABLED
		COREMIDI_ENABLED
	)
	target_link_libraries(rex_engine PUBLIC
		"$<LINK_LIBRARY:FRAMEWORK,Cocoa>"
		"$<LINK_LIBRARY:FRAMEWORK,Carbon>"
		"$<LINK_LIBRARY:FRAMEWORK,AudioUnit>"
		"$<LINK_LIBRARY:FRAMEWORK,CoreAudio>"
		"$<LINK_LIBRARY:FRAMEWORK,CoreMIDI>"
		"$<LINK_LIBRARY:FRAMEWORK,IOKit>"
		"$<LINK_LIBRARY:FRAMEWORK,GameController>"
		"$<LINK_LIBRARY:FRAMEWORK,CoreHaptics>"
		"$<LINK_LIBRARY:FRAMEWORK,CoreVideo>"
		"$<LINK_LIBRARY:FRAMEWORK,AVFoundation>"
		"$<LINK_LIBRARY:FRAMEWORK,CoreMedia>"
		"$<LINK_LIBRARY:FRAMEWORK,QuartzCore>"
		"$<LINK_LIBRARY:FRAMEWORK,Security>"
		"$<LINK_LIBRARY:FRAMEWORK,UniformTypeIdentifiers>"
	)
	target_link_libraries(rex_engine PUBLIC
		pthread
		z
	)

	set(EXTRA_FRAMEWORKS "")

	if (OPENGL3)
		target_compile_definitions(rex_engine PUBLIC GLES3_ENABLED)
		if (NOT ANGLE_LIBS STREQUAL "")
			target_compile_definitions(rex_engine PUBLIC EGL_STATIC)
			target_link_options(rex_engine PUBLIC
				-L${ANGLE_LIBS}
				-lANGLE.macos.${ARCH}
				-lEGL.macos.${ARCH}
				-lGLES.macos.${ARCH}
			)
			list(APPEND EXTRA_FRAMEWORKS "IOSurface")
		endif()
		target_include_directories(rex_engine PUBLIC
			${CMAKE_SOURCE_DIR}/thirdparty/angle/include
		)
	endif()

	set_property(
		TARGET rex_engine
		PROPERTY INSTALL_RPATH
		"@executable_path/../Frameworks"
		"@executable_path"
	)

	set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

	if (METAL AND NOT ARCH STREQUAL "arm64")
		message(WARNING "Target architecture '${ARCH}' does not support the Metal rendering driver")
		set(METAL OFF)
	endif()

	if (METAL)
		target_compile_definitions(rex_engine PUBLIC
			METAL_ENABLED
			RD_ENABLED
		)
		list(APPEND EXTRA_FRAMEWORKS
			Metal
			MetalKit
			MetalFX
		)
		target_include_directories(rex_engine PUBLIC
			${CMAKE_SOURCE_DIR}/thirdparty/spirv-cross
		)
	endif()

	if (VULKAN)
		target_compile_definitions(rex_engine PUBLIC
			VULKAN_ENABLED
			RD_ENABLED
		)
		list(APPEND EXTRA_FRAMEWORKS
			Metal
			IOSurface
		)
		if (NOT USE_VOLK)
			target_link_options(rex_engine PUBLIC -lMoltenVK)
			find_package(Vulkan COMPONENTS MoltenVK)
			if (Vulkan_MoltenVK_FOUND)
				target_link_options(rex_engine PUBLIC -L${Vulkan_MoltenVK_LIBRARY})
			else()
				message(FATAL_ERROR "MoltenVK SDK installation directory not found, use 'VULKAN_SDK_PATH' to specify SDK path.")
			endif()

		endif()
	endif()

	foreach(EXTRA_FRAMEWORK ${EXTRA_FRAMEWORKS})
		target_link_libraries(rex_engine PUBLIC "$<LINK_LIBRARY:FRAMEWORK,${EXTRA_FRAMEWORK}>")
	endforeach()
endif()
