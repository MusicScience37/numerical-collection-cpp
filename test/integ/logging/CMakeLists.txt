add_executable(num_collect_test_integ_logging_write_log_writer writer.cpp)
target_link_libraries(num_collect_test_integ_logging_write_log_writer
                      PRIVATE num_collect bfg::lyra Boost::headers)
target_add_ausan(num_collect_test_integ_logging_write_log_writer)

add_executable(num_collect_test_integ_logging_write_log
               write_logs_test.cpp ../../units/test_main.cpp)
target_add_catch2(num_collect_test_integ_logging_write_log)
target_add_ausan(num_collect_test_integ_logging_write_log)

# Without following, Boost.Process causes compile errors (not warnings).
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-c++11-narrowing
                        NUM_COLLECT_COMPILER_HAS_FLAG_WNO_CPP11_NARROWING)
if(NUM_COLLECT_COMPILER_HAS_FLAG_WNO_CPP11_NARROWING)
    target_compile_options(num_collect_test_integ_logging_write_log
                           PRIVATE -Wno-c++11-narrowing)
endif()
