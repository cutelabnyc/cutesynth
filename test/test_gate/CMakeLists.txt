add_executable(unity_gate test.gate.c)

target_link_libraries(unity_gate cutesynth Unity test_util)

add_test(unity_gate_test unity_gate)
