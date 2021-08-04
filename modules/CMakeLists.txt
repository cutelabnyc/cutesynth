file(GLOB MODULE_SOURCES ./*/*.c)

add_library(cutemodules STATIC ${MODULE_SOURCES})

target_include_directories(cutemodules INTERFACE ${CMAKE_CURRENT_LIST_DIR})
