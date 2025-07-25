file(RELATIVE_PATH CURRENT_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Mono Enabled")

set(MODULE_DOC_CLASSES
	CSharpScript
	GodotSharp
)
set(MODULE_DOC_CLASS_PATH "${CURRENT_RELATIVE_PATH}/doc_classes")

foreach(DOC_CLASS ${MODULE_DOC_CLASSES})
	list(APPEND DOC_CLASS_PATHS "${DOC_CLASS},${MODULE_DOC_CLASS_PATH}")
endforeach()

set(DOC_CLASS_PATHS ${DOC_CLASS_PATHS} PARENT_SCOPE)

add_library(rex_module_${MODULE_NAME} STATIC)
clone_library(rex_modules rex_module_${MODULE_NAME})
disable_warnings_for_target(rex_module_${MODULE_NAME})
set(MODULE_IS_ENABLED ON PARENT_SCOPE)

file(GLOB MONO_SOURCES
	*.cpp
	glue/*.cpp
	mono_gd/*.cpp
	utils/*.cpp
)
target_sources(rex_module_${MODULE_NAME} PRIVATE ${MONO_SOURCES})

if (EDITOR_BUILD)
	file(GLOB MONO_EDITOR_SOURCES editor/*.cpp)
	target_sources(rex_module_${MODULE_NAME} PRIVATE ${MONO_EDITOR_SOURCES})

	file(GLOB_RECURSE SCRIPT_TEMPLATES ${CMAKE_CURRENT_SOURCE_DIR}/editor/script_templates/*.cs)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/editor/script_templates/templates.gen.h
		COMMAND ${_Python3_EXECUTABLE}
		ARGS ${CMAKE_SOURCE_DIR}/generators/rex_template_builders.py make_templates
		${CMAKE_CURRENT_SOURCE_DIR}/editor/script_templates/templates.gen.h ${SCRIPT_TEMPLATES}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Mono Templates Header..."
	)
	add_custom_target(script_templates_mono DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/editor/script_templates/templates.gen.h)
	add_dependencies(file_generators script_templates_mono)
endif()

add_dependencies(rex_module_${MODULE_NAME} rex_module_regex)
