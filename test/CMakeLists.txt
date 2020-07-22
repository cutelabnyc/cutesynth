add_executable(cuteop_test test_tests.c)
 
target_link_libraries(cuteop_test cuteop Unity)
 
add_test(test_tests cuteop_test)
