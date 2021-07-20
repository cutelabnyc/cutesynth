add_executable(unity_thresh test.thresh.c)

target_link_libraries(unity_thresh cutesynth Unity test_util)

add_test(unity_thresh_test unity_thresh)
