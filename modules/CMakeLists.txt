file(GLOB SOURCES ./*/*.c)

add_library(modules STATIC ${SOURCES})

# target_link_libraries(cutesynth PRIVATE m)

target_include_directories(modules INTERFACE ${CMAKE_CURRENT_LIST_DIR})
