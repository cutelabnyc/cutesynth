file(GLOB SOURCES "./op/*.c")

add_library(cuteop STATIC ${SOURCES})

target_include_directories(cuteop INTERFACE ${CMAKE_CURRENT_LIST_DIR})
