include(./source_list.cmake)
add_executable(num_collect_test_integ_rbf_interp_comb
               ${SOURCE_FILES} ../catch_event_listner.cpp ../test_main.cpp)
target_compile_definitions(num_collect_test_integ_rbf_interp_comb
                           PRIVATE NUM_COLLECT_TEST_MODULE_NAME=rbf_interp_comb)
target_add_catch2(num_collect_test_integ_rbf_interp_comb)
target_add_ausan(num_collect_test_integ_rbf_interp_comb)

add_executable(num_collect_test_integ_rbf_interp_comb_unity EXCLUDE_FROM_ALL
               unity_source.cpp)
target_link_libraries(
    num_collect_test_integ_rbf_interp_comb_unity
    PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
            ApprovalTests::ApprovalTests num_collect num_prob_collect
            num_collect_test_utils)
