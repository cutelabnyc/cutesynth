add_executable(unity_series test.series.c)

target_link_libraries(unity_series cutesynth Unity test_util)

add_test(unity_series_test unity_series)
