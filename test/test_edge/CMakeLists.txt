add_executable(unity_edge test.edge.c)
 
target_link_libraries(unity_edge cuteop Unity)
 
add_test(unity_edge_test unity_edge)
