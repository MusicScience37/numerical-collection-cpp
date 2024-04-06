add_library(num_collect_test_utils STATIC src/table_comparator.cpp
                                          src/gzip_msgpack_output_stream.cpp)
target_include_directories(num_collect_test_utils
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    num_collect_test_utils
    PUBLIC Catch2::Catch2
           trompeloeil::trompeloeil
           ApprovalTests::ApprovalTests
           num_collect
           num_prob_collect
           cpp_msgpack_light::msgpack_light
           ZLIB::ZLIB)
