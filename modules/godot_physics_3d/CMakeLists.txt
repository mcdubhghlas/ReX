if (NOT DISABLE_PHYSICS_3D)
	add_library(rex_module_${MODULE_NAME} STATIC)
	clone_library(rex_modules rex_module_${MODULE_NAME})
	disable_warnings_for_target(rex_module_${MODULE_NAME})
	set(MODULE_IS_ENABLED ON PARENT_SCOPE)

	file(GLOB_RECURSE GODOT_PHYSICS_3D_SOURCES *.cpp)
	target_sources(rex_module_${MODULE_NAME} PRIVATE
		${GODOT_PHYSICS_3D_SOURCES}
	)
endif()
