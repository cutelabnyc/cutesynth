file(GLOB SOURCES ./src/*.c)

add_library(cuteop STATIC ${SOURCES})

target_include_directories(cuteop PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(cuteop INTERFACE ${CMAKE_CURRENT_LIST_DIR})
