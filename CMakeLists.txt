cmake_minimum_required(VERSION 4.0)

project(suckmore)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_executable(suckmore)
target_compile_features(suckmore PRIVATE cxx_std_20)
target_sources(suckmore PRIVATE 
    src/suckmore.cpp
)
target_include_directories(suckmore PRIVATE 
    src
    /home/anant/coding/StrangeLib/include/
)
target_link_libraries(suckmore PRIVATE 
    X11
    xcb
    /home/anant/coding/StrangeLib/build/libStrangeLib.a
)
target_compile_options(suckmore PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O2
    -g
)





install(TARGETS suckmore
    RUNTIME DESTINATION bin
)

