add_executable(unity_counter test.counter.c)
 
target_link_libraries(unity_counter cuteop Unity)
 
add_test(unity_counter_test unity_counter)
