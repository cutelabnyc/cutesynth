add_library(MissedOpportunities STATIC missed-opportunities/opportunity.c)

add_library(MessdUp STATIC messd-up/messd.c)

add_library(Uexkull STATIC uexkull/uexkull.c)

target_include_directories(Uexkull INTERFACE ${CMAKE_CURRENT_LIST_DIR}/uexkull)
