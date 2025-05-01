cmake_minimum_required(VERSION 3.15)
project(mydiscord-c C)

set(CMAKE_C_STANDARD 99)

# Forcer GCC sur Windows si nécessaire
if (WIN32)
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
endif()

# Ajout d’options de compilation pour GCC
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# === GTK3 ===
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

message(STATUS "GTK3 include dirs: ${GTK3_INCLUDE_DIRS}")
message(STATUS "GTK3 library dirs: ${GTK3_LIBRARY_DIRS}")
message(STATUS "GTK3 libraries: ${GTK3_LIBRARIES}")

# === Client ===
file(GLOB CLIENT_SOURCES "client/src/*.c")
add_executable(balqisse ${CLIENT_SOURCES})

# === Includes ===
target_include_directories(balqisse PRIVATE
    ${GTK3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/client/include
)

# === Définitions de compilation (ex : -D...) ===
target_compile_definitions(balqisse PRIVATE ${GTK3_CFLAGS_OTHER})

# === Lien ===
target_link_directories(balqisse PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries(balqisse PRIVATE ${GTK3_LIBRARIES})

# Verbose build (debug)
set(CMAKE_VERBOSE_MAKEFILE ON)
