file(GLOB SOURCES *.c)

add_library(test_util STATIC ${SOURCES})

target_link_libraries(test_util Unity)

target_include_directories(test_util INTERFACE ${CMAKE_CURRENT_LIST_DIR})
