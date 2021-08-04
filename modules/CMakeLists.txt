add_library(MissedOpportunities STATIC missed-opportunities/opportunity.c)
target_include_directories(MissedOpportunities PUBLIC missed-opportunities)

add_library(MessdUp STATIC messd-up/messd.c)
target_include_directories(MessdUp PUBLIC messd-up)

add_library(Uexkull STATIC uexkull/uexkull.c)
target_include_directories(Uexkull PUBLIC uexkull)
