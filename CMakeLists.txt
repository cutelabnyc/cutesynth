project ("CuteSynth" C)
cmake_minimum_required (VERSION 3.0)

add_subdirectory (src)
add_subdirectory (modules)

include(CTest)
add_subdirectory (thirdparty)
add_subdirectory (test)
