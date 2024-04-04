include(./source_list.cmake)
add_executable(num_collect_test_integ_linear_solvers
               ${SOURCE_FILES} ../catch_event_listner.cpp ../test_main.cpp)
target_compile_definitions(num_collect_test_integ_linear_solvers
                           PRIVATE NUM_COLLECT_TEST_MODULE_NAME=linear_solvers)
target_add_catch2(num_collect_test_integ_linear_solvers)
target_add_ausan(num_collect_test_integ_linear_solvers)

add_executable(num_collect_test_integ_linear_solvers_unity EXCLUDE_FROM_ALL
               unity_source.cpp)
target_link_libraries(
    num_collect_test_integ_linear_solvers_unity
    PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
            ApprovalTests::ApprovalTests num_collect num_prob_collect
            num_collect_test_utils)
