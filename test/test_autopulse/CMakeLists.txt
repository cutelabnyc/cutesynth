add_executable(unity_autopulse test.autopulse.c)

target_link_libraries(unity_autopulse cuteop Unity test_util)

add_test(unity_autopulse_test unity_autopulse)
