cmake_minimum_required(VERSION 3.20)
#程序名称
Set(PROJECT_NAME PDCurses)
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
set(CMAKE_CXX_STANDARD 14)

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

file(GLOB PDCURSES_PORT_C_CXX_FILES lib*.c lib*.h lib*.cpp)
add_library(${PROJECT_NAME} ${PDCURSES_PORT_C_CXX_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(${WASM_BUILD})
	#设定一些选项
	target_link_libraries(${PROJECT_NAME}    -sWASM_BIGINT=1 -sUSE_SDL=1 -sUSE_SDL_TTF=1 )
endif()

include(FindPkgConfig)
#添加线程库
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})

#添加PDCurses
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/PDCurses/pdcurses/ PDCursesCore EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PDCursesCore)
file(GLOB PDCURSES_SDL1_C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pdc*.c ${CMAKE_CURRENT_SOURCE_DIR}/pdc*.h)
target_sources(PDCursesCore  PRIVATE ${PDCURSES_SDL1_C_FILES})
if(${WASM_BUILD})
    #设定一些选项
    target_link_libraries(PDCursesCore  -sWASM_BIGINT=1 -sUSE_SDL=1 -sUSE_SDL_TTF=1 )
endif()


#sdl
include(FindSDL)
if(${SDL_FOUND})
	include_directories(${SDL_INCLUDE_DIRS})
	target_link_libraries(PDCursesCore SDL::SDL)
endif()

#sdl_image
include(FindSDL_image)
if(${SDL_IMAGE_FOUND})
	include_directories(${SDL_IMAGE_INCLUDE_DIRS})
	target_link_libraries(PDCursesCore ${SDL_IMAGE_LIBRARIES})
endif()

#sdl_mixer
include(FindSDL_mixer)
if(${SDL_MIXER_FOUND})
	include_directories(${SDL_MIXER_INCLUDE_DIRS})
	target_link_libraries(PDCursesCore ${SDL_MIXER_LIBRARIES})
endif()
#sdl_net
include(FindSDL_net)
if(${SDL_NET_FOUND})
	include_directories(${SDL_NET_INCLUDE_DIRS})
	target_link_libraries(PDCursesCore ${SDL_NET_LIBRARIES})
endif()
#sdl_sound
include(FindSDL_sound)
if(${SDL_SOUND_FOUND})
	include_directories(${SDL_SOUND_INCLUDE_DIRS})
	target_link_libraries(PDCursesCore ${SDL_SOUND_LIBRARIES})
endif()
#sdl_ttf
include(FindSDL_ttf)
if(${SDL_TTF_FOUND})
	include_directories(${SDL_TTF_INCLUDE_DIRS})
	target_link_libraries(PDCursesCore ${SDL_TTF_LIBRARIES})
endif()

#添加common_fsloader库文件
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common/fsloader common_fsloader EXCLUDE_FROM_ALL)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_fsloader)

#添加common_font库文件
set(FONT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fonts")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common/font common_font)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_font)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
target_compile_definitions(PDCursesCore PUBLIC PDC_FORCE_UTF8=1 PDC_WIDE=1 PDC_FONT_PATH=\"/font/DejaVuSansMono.ttf\")
else()
target_compile_definitions(PDCursesCore PUBLIC PDC_FORCE_UTF8=1 PDC_WIDE=1)
endif()


#添加3rdparty/HCppBox
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/HCppBox HCppBox EXCLUDE_FROM_ALL)
#启用HCppBox、HRC
hcppbox_enable(PDCursesCore)
hrc_enable(PDCursesCore)

include(GNUInstallDirs)
#添加Demos
set(DEMOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/PDCurses/demos/)

add_executable(firework ${DEMOS_DIR}/firework.c )
target_link_libraries(firework ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(firework  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/firework.html ${CMAKE_BINARY_DIR}/firework.data  ${CMAKE_BINARY_DIR}/firework.js ${CMAKE_BINARY_DIR}/firework.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()



add_executable(ozdemo  ${DEMOS_DIR}/ozdemo.c )
target_link_libraries(ozdemo ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(ozdemo  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/ozdemo.html  ${CMAKE_BINARY_DIR}/ozdemo.data ${CMAKE_BINARY_DIR}/ozdemo.js ${CMAKE_BINARY_DIR}/ozdemo.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()

add_executable(ptest  ${DEMOS_DIR}/ptest.c )
target_link_libraries(ptest ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(ptest  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/ptest.html ${CMAKE_BINARY_DIR}/ptest.data  ${CMAKE_BINARY_DIR}/ptest.js ${CMAKE_BINARY_DIR}/ptest.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()

add_executable(rain  ${DEMOS_DIR}/rain.c )
target_link_libraries(rain ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(rain  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/rain.html ${CMAKE_BINARY_DIR}/rain.data  ${CMAKE_BINARY_DIR}/rain.js ${CMAKE_BINARY_DIR}/rain.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()

add_executable(testcurs  ${DEMOS_DIR}/testcurs.c )
target_link_libraries(testcurs ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(testcurs  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/testcurs.html ${CMAKE_BINARY_DIR}/testcurs.data  ${CMAKE_BINARY_DIR}/testcurs.js ${CMAKE_BINARY_DIR}/testcurs.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()

add_executable(tuidemo  ${DEMOS_DIR}/tuidemo.c  ${DEMOS_DIR}/tui.c )
target_link_libraries(tuidemo ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(tuidemo  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/tuidemo.html  ${CMAKE_BINARY_DIR}/tuidemo.data  ${CMAKE_BINARY_DIR}/tuidemo.js ${CMAKE_BINARY_DIR}/tuidemo.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()

add_executable(worm  ${DEMOS_DIR}/worm.c )
target_link_libraries(worm ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(worm  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/worm.html  ${CMAKE_BINARY_DIR}/worm.data  ${CMAKE_BINARY_DIR}/worm.js ${CMAKE_BINARY_DIR}/worm.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()

add_executable(xmas  ${DEMOS_DIR}/xmas.c )
target_link_libraries(xmas ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(xmas  -sASYNCIFY  )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/xmas.html ${CMAKE_BINARY_DIR}/xmas.data  ${CMAKE_BINARY_DIR}/xmas.js ${CMAKE_BINARY_DIR}/xmas.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/
    )
endif()


add_executable(ModbusTCPClient  ${CMAKE_CURRENT_SOURCE_DIR}/ModbusTCPClient/main.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/ModbusTCPClient/ModbusSocketIo.h)
target_link_libraries(ModbusTCPClient ${PROJECT_NAME})
if(${WASM_BUILD})
    target_compile_definitions(ModbusTCPClient PUBLIC SOCKK=58080 )
    target_link_libraries(ModbusTCPClient -lwebsocket -sPROXY_POSIX_SOCKETS=1 -pthread -sPROXY_TO_PTHREAD=1 )
    INSTALL(FILES ${CMAKE_BINARY_DIR}/ModbusTCPClient.html ${CMAKE_BINARY_DIR}/ModbusTCPClient.data  ${CMAKE_BINARY_DIR}/ModbusTCPClient.js ${CMAKE_BINARY_DIR}/ModbusTCPClient.worker.js
 ${CMAKE_BINARY_DIR}/ModbusTCPClient.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/ModbusTCPClient/
    )
endif()

file(GLOB HSHELL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/HShell/*.cpp)
add_executable(HShell  ${HSHELL_FILES})
target_link_libraries(HShell ${PROJECT_NAME})
if(${WASM_BUILD})
    target_link_libraries(HShell  -sASYNCIFY)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/HShell.html ${CMAKE_BINARY_DIR}/HShell.data  ${CMAKE_BINARY_DIR}/HShell.js  ${CMAKE_BINARY_DIR}/HShell.wasm
    DESTINATION /var/www/html/wasm/${PROJECT_NAME}/HShell/
    )
endif()

