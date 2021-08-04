file(GLOB SOURCES ./*/*.c)

add_library(cutemodules STATIC ${SOURCES})

target_include_directories(cutemodules INTERFACE ${CMAKE_CURRENT_LIST_DIR})
