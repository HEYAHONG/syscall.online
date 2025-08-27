cmake_minimum_required(VERSION 3.10)

project(modbus_tcp_tiny_server)

file(GLOB sourceFiles *.cpp *.c *.h)

add_executable(modbus_tcp_tiny_server ${sourceFiles})

find_package(Threads)
target_link_libraries(modbus_tcp_tiny_server ${CMAKE_THREAD_LIBS_INIT})

#设定HCppBox使用纯C++
set(HCPPBOX_USE_PURE_CPLUSPLUS ON)

#设定HRC文件系统目录为fs
set(HRC_FS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fs/)
#添加3rdparty/HCppBox
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/HCppBox/  HCppBox EXCLUDE_FROM_ALL)
#启用HCppBox、HRC
hcppbox_enable(modbus_tcp_tiny_server)
hrc_enable(modbus_tcp_tiny_server)

include(FindPkgConfig)

pkg_check_modules(LIBEVENT REQUIRED IMPORTED_TARGET libevent)
target_link_libraries(modbus_tcp_tiny_server  PkgConfig::LIBEVENT)

pkg_check_modules(LIBEVENT_CORE REQUIRED IMPORTED_TARGET libevent_core)
target_link_libraries(modbus_tcp_tiny_server  PkgConfig::LIBEVENT_CORE)

pkg_check_modules(LIBEVENT_EXTRA REQUIRED IMPORTED_TARGET libevent_extra)
target_link_libraries(modbus_tcp_tiny_server  PkgConfig::LIBEVENT_EXTRA)

INSTALL(TARGETS modbus_tcp_tiny_server
       RUNTIME DESTINATION /usr/local/bin/
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
