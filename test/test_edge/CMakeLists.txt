add_executable(unity_edge test.edge.c)
 
target_link_libraries(unity_edge cuteop Unity test_util)
 
add_test(unity_edge_test unity_edge)
