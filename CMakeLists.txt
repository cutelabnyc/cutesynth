project ("CuteSynth" C)
cmake_minimum_required (VERSION 3.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory (src)
add_subdirectory (modules)

include(CTest)
add_subdirectory (thirdparty)
add_subdirectory (test)

