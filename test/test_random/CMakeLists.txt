add_executable(unity_random test.random.c)

target_link_libraries(unity_random cutesynth Unity test_util)

add_test(unity_random_test unity_random)
