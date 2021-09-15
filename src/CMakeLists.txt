file(GLOB SOURCES ./*/*.c)

add_library(cutesynth STATIC ${SOURCES})

# target_link_libraries(cutesynth PRIVATE m)

target_include_directories(cutesynth INTERFACE ${CMAKE_CURRENT_LIST_DIR})
