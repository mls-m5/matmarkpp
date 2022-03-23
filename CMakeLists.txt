
cmake_minimum_required(VERSION 3.22)

project(matmarkpp)

add_library(
    matmarklib
    src/matmark.cpp
    src/tables.cpp
    src/code.cpp
    )

target_include_directories(
    matmarklib
    PUBLIC
    include
    )

target_compile_features(
    matmarklib
    PUBLIC
    cxx_std_17
    )

add_executable(
    matmark
    main/main.cpp
    )

file(
    COPY
    README.md
    img.png
    DESTINATION
    .
    )

target_link_libraries(
    matmark
    PUBLIC
    matmarklib
    )
