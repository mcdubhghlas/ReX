file(RELATIVE_PATH CURRENT_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(MODULE_DOC_CLASSES
	ENetMultiplayerPeer
	ENetConnection
	ENetPacketPeer
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

if (BUILTIN_ENET)
	set(THIRDPARTY_ENET_DIR ${CMAKE_SOURCE_DIR}/thirdparty/enet/)
	add_library(enet OBJECT
        ${THIRDPARTY_ENET_DIR}enet_godot.cpp
        ${THIRDPARTY_ENET_DIR}callbacks.c
        ${THIRDPARTY_ENET_DIR}compress.c
        ${THIRDPARTY_ENET_DIR}host.c
        ${THIRDPARTY_ENET_DIR}list.c
        ${THIRDPARTY_ENET_DIR}packet.c
        ${THIRDPARTY_ENET_DIR}peer.c
        ${THIRDPARTY_ENET_DIR}protocol.c
	)
	target_include_directories(rex_module_${MODULE_NAME} PRIVATE
		${THIRDPARTY_ENET_DIR}
	)
	target_compile_definitions(rex_module_${MODULE_NAME} PRIVATE
		GODOT_ENET
	)

	clone_library(rex_module_${MODULE_NAME} enet)
	disable_warnings_for_target(enet)
	add_dependencies(enet rex_core)
	target_link_libraries(rex_module_${MODULE_NAME} PUBLIC enet)
endif()

file(GLOB ENET_SOURCES *.cpp)
target_sources(rex_module_${MODULE_NAME} PRIVATE
	${ENET_SOURCES}
)
