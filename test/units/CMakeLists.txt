set(NUM_COLLECT_TEST_UNITS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

function(add_unit_test_for MODULE_NAME)
    include(${NUM_COLLECT_TEST_UNITS_DIR}/${MODULE_NAME}/source_list.cmake)
    add_executable(num_collect_test_units_${MODULE_NAME}
                   ${SOURCE_FILES} ../catch_event_listner.cpp ../test_main.cpp)
    target_compile_definitions(
        num_collect_test_units_${MODULE_NAME}
        PRIVATE NUM_COLLECT_TEST_MODULE_NAME=${MODULE_NAME})
    target_add_catch2(num_collect_test_units_${MODULE_NAME})
    target_add_ausan(num_collect_test_units_${MODULE_NAME})

    add_executable(num_collect_test_units_${MODULE_NAME}_unity EXCLUDE_FROM_ALL
                   unity_source.cpp)
    target_link_libraries(
        num_collect_test_units_${MODULE_NAME}_unity
        PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
                ApprovalTests::ApprovalTests num_collect num_prob_collect)
    target_include_directories(num_collect_test_units_${MODULE_NAME}_unity
                               PRIVATE ${NUM_COLLECT_TEST_INCLUDE_DIR})
endfunction()

add_subdirectory(base)
add_subdirectory(util)
add_subdirectory(logging)
add_subdirectory(auto_diff)
add_subdirectory(opt)
add_subdirectory(regularization)
add_subdirectory(interp)
add_subdirectory(numbers)
add_subdirectory(multi_double)
add_subdirectory(constants)
add_subdirectory(roots)
add_subdirectory(functions)
add_subdirectory(integration)
add_subdirectory(ode)
