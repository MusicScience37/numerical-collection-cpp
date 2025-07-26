add_example(example_reg_sparse_image_regularization
            sparse_image_regularization.cpp)
target_link_libraries(example_reg_sparse_image_regularization
                      PRIVATE pngpp::pngpp)
