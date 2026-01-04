add_library(example_common STATIC src/configure_logging.cpp)
target_include_directories(example_common
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(example_common PUBLIC num_collect)
