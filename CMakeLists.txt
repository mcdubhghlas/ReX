cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

project(rex_engine
        VERSION 4.5
        DESCRIPTION "ReX Game Engine"
        LANGUAGES C CXX
)

# C Standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# export compile commands database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
	add_compile_options(/permissive- /Zc:__cplusplus)
elseif(MINGW)
	add_compile_options(-fpermissive)
endif()

# Used to force generation of files before compile
add_custom_target(file_generators)

include(methods.cmake)
include(version.cmake)

# TODO: Only checking for desktop platforms for now.
# NOTE: Using strings that match the Scons build system.
if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux|DragonFly|FreeBSD|NetBSD|OpenBSD")
	set(REX_PLATFORM "linuxbsd")
	execute_process( COMMAND uname -m OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
	set(REX_PLATFORM "macos")
	execute_process( COMMAND uname -m OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	set(REX_PLATFORM "windows")
	if(CMAKE_GENERATOR_PLATFORM MATCHES "x64")
		set(ARCH "x86_64")
	elseif(CMAKE_GENERATOR_PLATFORM MATCHES "Win32")
		set(ARCH "x86_32")
	else()
		# Fallback to CMAKE_SYSTEM_PROCESSOR
		if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
			set(ARCH "x86_64")
		elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
			set(ARCH "x86_32")
		else()
			message(WARNING "Unknown architecture: ${CMAKE_SYSTEM_PROCESSOR}. Assuming x64.")
			set(ARCH "x86_64")
		endif()
	endif()
else()
	message(FATAL_ERROR "${CMAKE_HOST_SYSTEM_NAME} not supported.")
endif()

message(STATUS "Detected build platform: ${REX_PLATFORM}")
message(STATUS "Architecture: ${ARCH}")

# For Cross-compilation support, check the host system architecture.
# TODO: This only support x86 platforms.
if (CMAKE_SIZEOF_VOID_P EQUAL 8 AND ARCH STREQUAL "x86_32")
	add_compile_options(-m32)
	add_link_options(-m32)
	message(STATUS "Cross-compilation detected.")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4 AND ARCH STREQUAL "x86_64")
	add_compile_options(-m64)
	add_link_options(-m64)
	message(STATUS "Cross-compilation detected.")
endif()

include(options.cmake)

if (NOT CMAKE_BUILD_TYPE)
	if (REX_DEV_BUILD OR DEBUG_SYMBOLS)
		set(CMAKE_BUILD_TYPE Debug)
	else()
		set(CMAKE_BUILD_TYPE Release)
	endif()
endif()

if (BUILD_TARGET STREQUAL "editor")
	set(EDITOR_BUILD ON)
endif()

if (BUILD_TARGET STREQUAL "editor" OR BUILD_TARGET STREQUAL "template_debug")
	set(DEBUG_FEATURES ON)
endif()

# 'DEV_MODE' and 'PRODUCTION' are aliases to set default options if they haven't been
# set manually by the user.
if (DEV_MODE)
	set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose build output" FORCE)
	set(REX_TESTS ON CACHE BOOL "Build the unit tests" FORCE)
	set(WARNING "extra" CACHE STRING "Level of compilation warnings: extra, all, moderate, no" FORCE)
	set(WERROR ON CACHE BOOL "Treat compiler warnings as errors" FORCE)
	set(STRICT_CHECKS ON CACHE BOOL "Enforce stricter checks (debug option)" FORCE)
endif()

if (PRODUCTION)
	set(USE_STATIC_CPP ON CACHE BOOL "Link runtime libraries statically" FORCE)
	set(DEBUG_SYMBOLS OFF CACHE BOOL "Build with debugging symbols" FORCE)
	set(LTO "auto" CACHE STRING "Link-time optimization (production builds): none, auto, thin, full" FORCE)
endif()

# The executable target 'rex_engine' is defined within the target platforms subdirectory
add_subdirectory(platform/linuxbsd)
add_subdirectory(platform/macos)
add_subdirectory(platform/windows)

target_include_directories(rex_engine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

if (EDITOR_BUILD)
	target_compile_definitions(rex_engine PUBLIC TOOLS_ENABLED)
	if (ENGINE_UPDATE_CHECK)
		target_compile_definitions(rex_engine PUBLIC ENGINE_UPDATE_CHECK_ENABLED)
	endif()
	if (NOT EXISTS ${CMAKE_SOURCE_DIR}/main/splash_editor.png)
		set(NO_EDITOR_SPLASH ON)
	endif()
	if (NO_EDITOR_SPLASH)
		target_compile_definitions(rex_engine PUBLIC NO_EDITOR_SPLASH)
	endif()
endif()

if (DEBUG_FEATURES)
    # DEBUG_ENABLED enables debugging *features* and debug-only code, which is intended
    # to give *users* extra debugging information for their game development.
	target_compile_definitions(rex_engine PUBLIC DEBUG_ENABLED)
endif()

if (REX_DEV_BUILD)
    # DEV_ENABLED enables *engine developer* code which should only be compiled for those
    # working on the engine itself.
	set(DEBUG_SYMBOLS ON)
	target_compile_definitions(rex_engine PUBLIC DEV_ENABLED)
else()
    # Disable assert() for production targets (only used in thirdparty code).
	target_compile_definitions(rex_engine PUBLIC NDEBUG)
endif()

if (USE_PRECISE_MATH_CHECKS)
	target_compile_definitions(rex_engine PUBLIC PRECISE_MATH_CHECKS)
endif()

if (NOT DEPRICATED)
	target_compile_definitions(rex_engine PUBLIC DISABLE_DEPRECATED)
endif()

if (PRECISION STREQUAL "double")
	target_compile_definitions(rex_engine PUBLIC REAL_T_IS_DOUBLE)
endif()

if (STRICT_CHECKS)
	target_compile_definitions(rex_engine PUBLIC STRICT_CHECKS)
endif()


# TODO: Missing SCU Build here


if (EDITOR_BUILD)
	if (DISABLE_3D
	 OR DISABLE_ADVANCED_GUI
	 OR DISABLE_PHYSICS_2D
	 OR DISABLE_PHYSICS_3D
	 OR DISABLE_NAVIGATION_2D
	 OR DISABLE_NAVIGATION_3D)
		message(FATAL_ERROR "The following build options cannot be used for editor builds, DISABLE_3D, DISABLE_ADVANCED_GUI,
 DISABLE_PHYSICS_2D, DISABLE_PHYSICS_3D, DISABLE_NAVIGATION_2D, DISABLE_NAVIGATION_3D")
	endif()
endif()

if (DISABLE_3D)
	target_compile_definitions(rex_engine PUBLIC _3D_DISABLED)
	set(DISABLE_NAVIGATION_3D ON)
	set(DISABLE_PHYSICS_3D ON)
	set(DISABLE_XR ON)
endif()

if (DISABLE_ADVANCED_GUI)
	target_compile_definitions(rex_engine PUBLIC ADVANCED_GUI_DISABLED)
endif()

if (DISABLE_PHYSICS_2D)
	target_compile_definitions(rex_engine PUBLIC PHYSICS_2D_DISABLED)
endif()

if (DISABLE_PHYSICS_3D)
	target_compile_definitions(rex_engine PUBLIC PHYSICS_3D_DISABLED)
endif()

if (DISABLE_NAVIGATION_2D)
	target_compile_definitions(rex_engine PUBLIC NAVIGATION_2D_DISABLED)
endif()

if (DISABLE_NAVIGATION_3D)
	target_compile_definitions(rex_engine PUBLIC NAVIGATION_3D_DISABLED)
endif()

if (DISABLE_XR)
	target_compile_definitions(rex_engine PUBLIC XR_DISABLED)
endif()

if (MINIZIP)
	target_compile_definitions(rex_engine PUBLIC MINIZIP_ENABLED)
endif()

if (BROTLI)
	target_compile_definitions(rex_engine PUBLIC BROTLI_ENABLED)
endif()

if (THREADS)
	target_compile_definitions(rex_engine PUBLIC THREADS_ENABLED)
endif()

# Disable exception handling. Redot doesn't use exceptions anywhere, and this
# saves around 20% of binary size and very significant build time (GH-80513).
if (DISABLE_EXCEPTIONS)
	if (MSVC)
		target_compile_definitions(rex_engine PUBLIC _HAS_EXCEPTIONS=0)
	else()
		target_compile_options(rex_engine PUBLIC -fno-exceptions)
	endif()
elseif(MSVC)
	target_compile_options(rex_engine PUBLIC /EHsc)
endif()

# Output name
set(OUTPUT_NAME "rex.${REX_PLATFORM}")
if(EDITOR_BUILD)
    set(OUTPUT_NAME "${OUTPUT_NAME}.editor")
else()
    set(OUTPUT_NAME "${OUTPUT_NAME}.opt")
endif()
if(REX_BUILD_TYPE MATCHES "debug")
    set(OUTPUT_NAME "${OUTPUT_NAME}.debug")
endif()
set_target_properties(rex_engine PROPERTIES OUTPUT_NAME "${OUTPUT_NAME}")

if (OPTIMIZE STREQUAL "size_extra")
	target_compile_definitions(rex_engine PUBLIC SIZE_EXTRA)
endif()

if (ARCH STREQUAL "x86_32")
	if (MSVC)
		target_compile_options(rex_engine PUBLIC
			/arch:SSE2
		)
	else()
		target_compile_options(rex_engine PUBLIC
			-msse2
			-mfpmath=sse
			-mstackrealign
		)
	endif()
endif()
# Compiler flags
# Set optimize and debug_symbols flags.
# "custom" means do nothing and let users set their own optimization flags.
if (MSVC)
	if (DEBUG_SYMBOLS)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zi /FS")
		target_link_options(rex_engine PUBLIC /DEBUG:FULL)
	else()
		target_link_options(rex_engine PUBLIC /DEBUG:NONE)
	endif()
	if (OPTIMIZE MATCHES "speed")
		target_compile_options(rex_engine PUBLIC -O2)
		target_link_options(rex_engine PUBLIC /OPT:REF)
		if (OPTIMIZE STREQUAL "speed_trace")
			target_link_options(rex_engine PUBLIC /OPT:NOICF)
		endif()
	elseif (OPTIMIZE MATCHES "size")
		target_compile_options(rex_engine PUBLIC -O1)
		target_link_options(rex_engine PUBLIC /OPT:REF)
		if (OPTIMIZE STREQUAL "speed_trace")
			target_compile_definitions(rex_engine PUBLIC SIZE_EXTRA)
		endif()
	else()
		target_compile_options(rex_engine PUBLIC -Od)
	endif()
else()
	if (DEBUG_SYMBOLS)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf-4")
		if (REX_DEV_BUILD)
			target_compile_options(rex_engine PUBLIC -g3)
		else()
			target_compile_options(rex_engine PUBLIC -g2)
		endif()
    endif()

	if (OPTIMIZE STREQUAL "speed")
		target_compile_options(rex_engine PUBLIC -O3)
	elseif (OPTIMIZE STREQUAL "speed_trace")
		target_compile_options(rex_engine PUBLIC -O2)
	elseif (OPTIMIZE MATCHES "size")
		target_compile_options(rex_engine PUBLIC -Os)
	elseif (OPTIMIZE STREQUAL "debug")
		target_compile_options(rex_engine PUBLIC -Og)
	elseif (OPTIMIZE STREQUAL "none")
		target_compile_options(rex_engine PUBLIC -O0)
	endif()
endif()

# Output binary directory
set_target_properties(rex_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# These subdirectories need to be in this order for linking to work.
add_subdirectory(core)
add_subdirectory(servers)
add_subdirectory(scene)
if (BUILD_TARGET STREQUAL "editor")
	add_subdirectory(editor)
endif()
add_subdirectory(drivers)
add_subdirectory(platform)
add_subdirectory(main)
if (REX_TESTS)
	add_subdirectory(tests)
endif()
add_subdirectory(modules)


# This is placed here so the gathering of doc paths can be part of CMake iterating through the sub directories
if (EDITOR_BUILD)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/doc_data_class_path.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_builders.py doc_data_class_path_builder
		${CMAKE_SOURCE_DIR}/editor/doc_data_class_path.gen.h ${DOC_CLASS_PATHS}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Doc Data Class Path Header..."
	)
	add_custom_target(doc_data_class_path DEPENDS ${CMAKE_SOURCE_DIR}/editor/doc_data_class_path.gen.h)
	add_dependencies(file_generators doc_data_class_path)

	file(GLOB DOCS ${CMAKE_SOURCE_DIR}/doc/classes/*.xml)
	set(MODULE_DIRS)
	foreach(DOC_PATH ${DOC_CLASS_PATHS})
		list(FIND MODULE_DIRS ${DOC_PATH} DIR_INDEX)
		if (DIR_INDEX EQUAL -1)
			list(APPEND MODULE_DIRS ${DOC_PATH})
		endif()
	endforeach()
	foreach(DIR ${MODULE_DIRS})
		file(GLOB MODULE_DOCS ${DIR}/*.xml)
		list(APPEND DOCS ${MODULE_DOCS})
	endforeach()
	set(DOCS_LIST_FILE ${CMAKE_BINARY_DIR}/docs_list.txt)
	file(GENERATE
		OUTPUT "${DOCS_LIST_FILE}"
		CONTENT "$<JOIN:${DOCS},\n>"
	)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/doc_data_compressed.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_builders.py make_doc_header
		${CMAKE_SOURCE_DIR}/editor/doc_data_compressed.gen.h ${DOCS_LIST_FILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Doc Data Compressed Header..."
	)
	add_custom_target(doc_data_compressed DEPENDS ${CMAKE_SOURCE_DIR}/editor/doc_data_compressed.gen.h)
	add_dependencies(file_generators doc_data_compressed)

	file(GLOB EDITOR_TRANSLATIONS ${CMAKE_SOURCE_DIR}/editor/translations/editor/*)
	set(EDITOR_TRANSLATIONS_LIST_FILE ${CMAKE_BINARY_DIR}/editor_translations_list.txt)
	file(GENERATE
		OUTPUT "${EDITOR_TRANSLATIONS_LIST_FILE}"
		CONTENT "$<JOIN:${EDITOR_TRANSLATIONS},\n>"
	)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/editor_translations.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_builders.py make_translations_header
		${CMAKE_SOURCE_DIR}/editor/editor_translations.gen.h ${EDITOR_TRANSLATIONS_LIST_FILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Editor Translations Header..."
	)
	add_custom_target(editor_translations DEPENDS ${CMAKE_SOURCE_DIR}/editor/editor_translations.gen.h)
	add_dependencies(file_generators editor_translations)

	file(GLOB PROPERTIES_TRANSLATIONS ${CMAKE_SOURCE_DIR}/editor/translations/properties/*)
	set(PROPERTIES_TRANSLATIONS_LIST_FILE ${CMAKE_BINARY_DIR}/properties_translations_list.txt)
	file(GENERATE
		OUTPUT "${PROPERTIES_TRANSLATIONS_LIST_FILE}"
		CONTENT "$<JOIN:${PROPERTIES_TRANSLATIONS},\n>"
	)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/property_translations.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_builders.py make_translations_header
		${CMAKE_SOURCE_DIR}/editor/property_translations.gen.h ${PROPERTIES_TRANSLATIONS_LIST_FILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Properties Translations Header..."
	)
	add_custom_target(properties_translations DEPENDS ${CMAKE_SOURCE_DIR}/editor/property_translations.gen.h)
	add_dependencies(file_generators properties_translations)

	file(GLOB DOC_TRANSLATIONS ${CMAKE_SOURCE_DIR}/doc/translations/*)
	set(DOC_TRANSLATIONS_LIST_FILE ${CMAKE_BINARY_DIR}/doc_translations_list.txt)
	file(GENERATE
		OUTPUT "${DOC_TRANSLATIONS_LIST_FILE}"
		CONTENT "$<JOIN:${DOC_TRANSLATIONS},\n>"
	)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/doc_translations.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_builders.py make_translations_header
		${CMAKE_SOURCE_DIR}/editor/doc_translations.gen.h ${DOC_TRANSLATIONS_LIST_FILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Doc Translations Header..."
	)
	add_custom_target(doc_translations DEPENDS ${CMAKE_SOURCE_DIR}/editor/doc_translations.gen.h)
	add_dependencies(file_generators doc_translations)

	file(GLOB EXTRACTABLE_TRANSLATIONS ${CMAKE_SOURCE_DIR}/editor/translations/extractable/*)
	set(EXTRACTABLE_TRANSLATIONS_LIST_FILE ${CMAKE_BINARY_DIR}/extractable_translations_list.txt)
	file(GENERATE
		OUTPUT "${EXTRACTABLE_TRANSLATIONS_LIST_FILE}"
		CONTENT "$<JOIN:${EXTRACTABLE_TRANSLATIONS},\n>"
	)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/extractable_translations.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_builders.py make_translations_header
		${CMAKE_SOURCE_DIR}/editor/extractable_translations.gen.h ${EXTRACTABLE_TRANSLATIONS_LIST_FILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Extractable Translations Header..."
	)
	add_custom_target(extractable_translations DEPENDS ${CMAKE_SOURCE_DIR}/editor/extractable_translations.gen.h)
	add_dependencies(file_generators extractable_translations)

	# Editor Icons
	file(GLOB EDITOR_ICONS ${CMAKE_SOURCE_DIR}/editor/icons/*.svg)
	foreach(MODULE_ICON_PATH ${ICON_PATHS})
		file(GLOB MODULE_ICONS ${MODULE_ICON_PATH}/*.svg)
		list(APPEND EDITOR_ICONS ${MODULE_ICONS})
	endforeach()
	set(EDITOR_ICONS_LIST_FILE ${CMAKE_BINARY_DIR}/editor_icons_list.txt)
	file(GENERATE
		OUTPUT "${EDITOR_ICONS_LIST_FILE}"
		CONTENT "$<JOIN:${EDITOR_ICONS},\n>"
	)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/editor/themes/editor_icons.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_editor_icons_builders.py make_editor_icons_action
		${CMAKE_SOURCE_DIR}/editor/themes/editor_icons.gen.h ${EDITOR_ICONS_LIST_FILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Editor Icons Header..."
	)
	add_custom_target(editor_icons DEPENDS ${CMAKE_SOURCE_DIR}/editor/themes/editor_icons.gen.h)
	add_dependencies(file_generators editor_icons)

endif()

target_link_libraries(rex_engine PUBLIC
	rex_platform
	rex_drivers
)
if (BUILD_TARGET STREQUAL "editor")
	target_link_libraries(rex_engine PUBLIC rex_editor)
endif()

list(REMOVE_DUPLICATES SYSTEM_LIBRARIES)

target_link_libraries(rex_engine PUBLIC
	rex_scene
	rex_servers
	rex_core
	${BUILTIN_LIBRARIES}
	${SYSTEM_LIBRARIES}
)
