cmake_minimum_required(VERSION 3.10)

project(websocket_to_posix_proxy)

if (WIN32)
  if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/wd4200) # "nonstandard extension used: zero-sized array in struct/union"
  endif()
else()
  add_compile_options(-Werror -Wall)
endif()

file(GLOB sourceFiles src/*.cpp src/*.c src/*.h)

add_executable(websocket_to_posix_proxy ${sourceFiles})

find_package(Threads)
target_link_libraries(websocket_to_posix_proxy ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
  target_link_libraries(websocket_to_posix_proxy ws2_32.lib)
endif()

INSTALL(TARGETS websocket_to_posix_proxy
       RUNTIME DESTINATION /usr/local/bin/
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
