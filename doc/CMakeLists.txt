message("Configure documentation of num_collect")

add_custom_target(num_collect_doc)

set(NUM_COLLECT_HTML_ROOT ${CMAKE_CURRENT_BINARY_DIR}/html)

add_subdirectory(doxygen)

option(NUM_COLLECT_BUILD_DOC_SPHINX
       "build documentation of num_collect with Sphinx" ON)
if(NUM_COLLECT_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()
