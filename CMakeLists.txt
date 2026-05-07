cmake_minimum_required(VERSION 3.20)

project(suckmore)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_BUILD_TYPE Debug)

add_executable(suckmore)
target_sources(suckmore PRIVATE 
    ./src/suckmore.cpp
    ./src/util.cpp
    ./src/layouts/MStack.cpp
    ./src/ds/CirDLinkedList.cpp
)
target_include_directories(suckmore PRIVATE 
    ./src
    ./src/ds/
    ./src/layouts/
)
target_link_libraries(suckmore PRIVATE 
    X11
    xcb
)
target_compile_options(suckmore PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O0
    -g3
    -ggdb
)

# Optional: Strip prefix for debug paths
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(suckmore PRIVATE -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.)
endif()


install(TARGETS suckmore
    RUNTIME DESTINATION bin
)

