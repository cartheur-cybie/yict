cmake_minimum_required(VERSION 3.16)
project(yict_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()

add_library(yict_core
    src/yict_core.cpp
)

target_include_directories(yict_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(yict_cli
    src/main.cpp
)

target_link_libraries(yict_cli
    PRIVATE
        yict_core
)

add_executable(yict_core_tests
    tests/test_yict_core.cpp
)

target_link_libraries(yict_core_tests
    PRIVATE
        yict_core
)

target_compile_definitions(yict_core_tests
    PRIVATE
        YICT_REPO_ROOT=\"${CMAKE_SOURCE_DIR}/..\"
)

add_test(NAME yict_core_tests COMMAND yict_core_tests)
