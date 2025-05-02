cmake_minimum_required(VERSION 3.15)
project(mydiscord-c C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (WIN32)
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
endif()


if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()


find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

message(STATUS "GTK3 include dirs: ${GTK3_INCLUDE_DIRS}")
message(STATUS "GTK3 libraries: ${GTK3_LIBRARIES}")


file(GLOB CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/client/src/*.c")
file(GLOB CLIENT_SOURCES "client/src/*.c")


add_executable(mydiscord_client ${CLIENT_SOURCES})


target_include_directories(mydiscord_client PRIVATE
    ${GTK3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/client/include
)


target_compile_definitions(mydiscord_client PRIVATE ${GTK3_CFLAGS_OTHER})


target_link_libraries(mydiscord_client PRIVATE ${GTK3_LIBRARIES})

file(GLOB CLIENT_SOURCES "client/src/*.c")

