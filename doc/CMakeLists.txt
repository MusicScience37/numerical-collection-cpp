message("Configure documentation of ${FULL_PROJECT_NAME}")

add_custom_target(num_collect_doc)

set(NUM_COLLECT_HTML_ROOT ${CMAKE_CURRENT_BINARY_DIR}/html)

add_subdirectory(doxygen)

option(NUM_COLLECT_BUILD_DOC_SPHINX
       "build documentation of ${FULL_PROJECT_NAME} with Sphinx" ON)
if(NUM_COLLECT_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()
