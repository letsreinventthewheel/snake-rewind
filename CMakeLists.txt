cmake_minimum_required(VERSION 3.25)
project(snake-rewind C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
