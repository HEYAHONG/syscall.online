
include(GNUInstallDirs)
#添加common_font库文件
file(GLOB FONT_CXX_C_FILES *.cpp  *.c *.h)
add_library(common_font STATIC  EXCLUDE_FROM_ALL ${FONT_CXX_C_FILES})
target_include_directories(common_font PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#判断是否为Emscripten
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	target_link_libraries(common_font "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/font/@font/" )
	target_compile_definitions(common_font PUBLIC FONT_SOURCE_ROOT="${CMAKE_SOURCE_DIR}" FONT_ROOT_DIR="/font/")
else()
	#本地测试通过安装字体实现测试
	target_compile_definitions(common_font PUBLIC FONT_SOURCE_ROOT="${CMAKE_SOURCE_DIR}" FONT_ROOT_DIR="${CMAKE_INSTALL_FULL_DATADIR}/fonts/")
	file(GLOB FONT_TTF_FILES font/*.ttf)
	message(STATUS "install ${FONT_TTF_FILES} to ${CMAKE_INSTALL_FULL_DATADIR}/fonts/")
	install(FILES ${FONT_TTF_FILES}
		      DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/fonts/)
endif()
