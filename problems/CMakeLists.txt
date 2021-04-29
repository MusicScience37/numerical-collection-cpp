add_library(num_prob_collect INTERFACE)
target_include_directories(num_prob_collect
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(num_prob_collect INTERFACE num_collect)
