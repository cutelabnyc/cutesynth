project ("Cuteop Library" C)
cmake_minimum_required (VERSION 3.0)

add_subdirectory (cuteop)

include(CTest)
add_subdirectory (thirdparty) 
add_subdirectory (test) 