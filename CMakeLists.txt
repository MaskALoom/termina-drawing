cmake_minimum_required(VERSION 3.23)
project(termina_drawing)
find_package(raylib REQUIRED)

find_package(SDL3 REQUIRED CONFIG)

add_executable(r)
target_include_directories(r
    PRIVATE
    /usr/local/include
)
target_link_directories(r
    PRIVATE
    /usr/local/lib
)
target_link_libraries(r PRIVATE cjson)
target_link_libraries(r PRIVATE raylib)
target_link_libraries(r PRIVATE SDL3::SDL3)
target_sources(r
    PRIVATE
    src/main.cpp
)
