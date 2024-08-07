cmake_minimum_required(VERSION 3.20)
#程序名称
Set(PROJECT_NAME base_sdl)
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

#sdl
include(FindSDL)
if(${SDL_FOUND})
	target_link_libraries(${PROJECT_NAME} SDL::SDL)
endif()

#sdl_image
include(FindSDL_image)
if(${SDL_IMAGE_FOUND})
	target_link_libraries(${PROJECT_NAME} ${SDL_IMAGE_LIBRARIES})
endif()

#sdl_mixer
include(FindSDL_mixer)
if(${SDL_MIXER_FOUND})
	target_link_libraries(${PROJECT_NAME} ${SDL_MIXER_LIBRARIES})
endif()
#sdl_net
include(FindSDL_net)
if(${SDL_NET_FOUND})
	target_link_libraries(${PROJECT_NAME} ${SDL_NET_LIBRARIES})
endif()
#sdl_sound
include(FindSDL_sound)
if(${SDL_SOUND_FOUND})
	target_link_libraries(${PROJECT_NAME} ${SDL_SOUND_LIBRARIES})
endif()
#sdl_ttf
include(FindSDL_ttf)
if(${SDL_TTF_FOUND})
	target_link_libraries(${PROJECT_NAME} ${SDL_TTF_LIBRARIES})
endif()

if(${WASM_BUILD})
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.html ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.js ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.wasm
       DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
)
endif()


