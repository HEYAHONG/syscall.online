
include(GNUInstallDirs)
add_library(common_shell_console INTERFACE)
add_library(common_shell_gui     INTERFACE)
#判断是否为Emscripten
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    target_link_libraries(common_shell_console INTERFACE "--shell-file ${CMAKE_CURRENT_LIST_DIR}/shell_console.html")
    target_link_libraries(common_shell_gui     INTERFACE "--shell-file ${CMAKE_CURRENT_LIST_DIR}/shell_gui.html")
else()
	
endif()
