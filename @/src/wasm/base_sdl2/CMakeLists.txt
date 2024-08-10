cmake_minimum_required(VERSION 3.20)
#程序名称
Set(PROJECT_NAME base_sdl2)
#主版本
set(PROJECT_MAJOR_VERSION_STR 1)
#次版本号
set(PROJECT_MINOR_VERSION_STR 0)
#修订号
set(PROJECT_REVISION_VERSION_STR 0)
#构建版本（以时间戳天数作为构建版本）
string(TIMESTAMP PROJECT_BUILD_TIME_SECOND "%s" UTC)
math(EXPR  PROJECT_BUILD_VERSION_STR "${PROJECT_BUILD_TIME_SECOND}/60/60/24"  OUTPUT_FORMAT DECIMAL)
#程序版本
set(PROJECT_VERSION_STR "${PROJECT_MAJOR_VERSION_STR}.${PROJECT_MINOR_VERSION_STR}.${PROJECT_REVISION_VERSION_STR}.${PROJECT_BUILD_VERSION_STR}")
#工程根目录
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})


Project(${PROJECT_NAME}	VERSION "${PROJECT_VERSION_STR}")
Project(${PROJECT_NAME} C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

#判断是否为Emscripten
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	set(WASM_BUILD ON)
	message(STATUS "This is a wasm build!")
else()
	set(WASM_BUILD OFF)
endif()

if(${WASM_BUILD})
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	add_compile_definitions(WASM_BUILD=1)
endif()

file(GLOB MAIN_CXX_C_FILES *.cpp  *.c *.h)
add_executable(${PROJECT_NAME}  ${MAIN_CXX_C_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(FindPkgConfig)
#添加线程库
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})

include(FindPkgConfig)
#sdl2
find_package(sdl2 REQUIRED)
if(${SDL2_SDL2main_FOUND})
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2::SDL2main)
endif()
if(${SDL2_SDL2test_FOUND})
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2TEST_LIBRARY})
endif()
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  SDL2::SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

if(NOT ${WASM_BUILD})
	#sdl2_image
	find_package(sdl2_image REQUIRED)
	if(${SDL2_image_FOUND})
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2_image::SDL2_image)
	endif()
	#sdl2_mixer
	find_package(sdl2_mixer REQUIRED)
	if(${SDL2_mixer_FOUND})
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2_mixer::SDL2_mixer)
	endif()
	#sdl2_net
	find_package(sdl2_net REQUIRED)
	if(${SDL2_net_FOUND})
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2_net::SDL2_net)
	endif()
	#sdl2_ttf
        find_package(sdl2_ttf REQUIRED)
	if(${SDL2_ttf_FOUND})
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2_ttf::SDL2_ttf)
	endif()
endif()

if(${WASM_BUILD})
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.html ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.js ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.wasm
       DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
)
endif()


