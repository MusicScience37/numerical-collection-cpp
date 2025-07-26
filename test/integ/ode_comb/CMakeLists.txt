include(./source_list.cmake)
add_executable(test_integ_ode_comb ${SOURCE_FILES} ../catch_event_listner.cpp
                                   ../test_main.cpp)
target_compile_definitions(test_integ_ode_comb
                           PRIVATE NUM_COLLECT_TEST_MODULE_NAME=ode_comb)
target_add_catch2(test_integ_ode_comb)
target_add_ausan(test_integ_ode_comb)

add_executable(test_integ_ode_comb_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(
    test_integ_ode_comb_unity
    PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
            ApprovalTests::ApprovalTests num_collect num_prob_collect
            num_collect_test_utils)
