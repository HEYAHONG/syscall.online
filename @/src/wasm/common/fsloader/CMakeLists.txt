
#添加common::fsloader库文件
file(GLOB FSLOADER_CXX_C_FILES *.cpp  *.c *.h)
add_library(common_fsloader STATIC  EXCLUDE_FROM_ALL ${FSLOADER_CXX_C_FILES})
target_include_directories(common_fsloader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(common_fsloader PUBLIC FSLOADER_SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
#判断是否为Emscripten
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	target_link_libraries(common_fsloader -sFORCE_FILESYSTEM -sFILESYSTEM=1  -lnodefs.js -lidbfs.js)
endif()
