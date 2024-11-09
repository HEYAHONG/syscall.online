cmake_minimum_required(VERSION 3.8)
#程序名称
Set(PROJECT_NAME syscalld)
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
Project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_CXX_STANDARD 20)
file(GLOB MAIN_CXX_C_FILES *.cpp  *.c *.h)
add_executable(${PROJECT_NAME}  ${MAIN_CXX_C_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#添加线程库
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})


include(FindPkgConfig)

#libwebsockets
pkg_check_modules(LIBWEBSOCKETS REQUIRED IMPORTED_TARGET libwebsockets)
target_link_libraries(${PROJECT_NAME} PkgConfig::LIBWEBSOCKETS)


#jsoncpp
pkg_check_modules(JSONCPP REQUIRED IMPORTED_TARGET jsoncpp)
target_link_libraries(${PROJECT_NAME} PkgConfig::JSONCPP)

#dbus-1
pkg_check_modules(DBUS REQUIRED IMPORTED_TARGET dbus-1)
target_link_libraries(${PROJECT_NAME} PkgConfig::DBUS)

#Boost
find_package(Boost 1.74.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} Boost::boost)

#添加3rdparty/HCppBox
add_compile_options( -DHCPPRT_NO_NEW_AND_DELETE_OVERRIDE=1 )
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/HCppBox HCppBox EXCLUDE_FROM_ALL)
#启用HCppBox、HRC
hcppbox_enable(${PROJECT_NAME})
hrc_enable(${PROJECT_NAME})

include(GNUInstallDirs)
#安装
INSTALL(TARGETS ${PROJECT_NAME}
       RUNTIME DESTINATION /usr/local/bin/
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#安装syscalld的systemd配置文件
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/systemd/syscalld.service
       DESTINATION /lib/systemd/system/
)

#安装syscalld的dbus配置文件
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dbus/syscalld.conf
       DESTINATION /etc/dbus-1/system.d/
)


#包含子目录
include(${PROJECT_ROOT_DIR}/cmake/subdir.cmake)
