# add_library(MissedOpportunities STATIC missed-opportunities/opportunity.c)

# add_library(MessdUp STATIC messd-up/messd.c)

# add_library(Uexkull STATIC uexkull/uexkull.c)

# target_include_directories(Uexkull INTERFACE ${CMAKE_CURRENT_LIST_DIR})

file(GLOB MODULE_SOURCES ./*/*.c)

add_library(cutemodules STATIC ${MODULE_SOURCES})

target_include_directories(cutemodules INTERFACE ${CMAKE_CURRENT_LIST_DIR})
