message("Configure documentation of ${FULL_PROJECT_NAME}")

add_custom_target(num_collect_doc)

set(NUM_COLLECT_HTML_ROOT
    ${CMAKE_CURRENT_BINARY_DIR}/html
    CACHE STRING "directory to write documentation" FORCE)

add_subdirectory(doxygen)

option(NUM_COLLECT_BUILD_DOC_SPHINX
       "build documentation of ${FULL_PROJECT_NAME} with Sphinx" ON)
if(NUM_COLLECT_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()

file(MAKE_DIRECTORY ${NUM_COLLECT_HTML_ROOT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/icon_logo/icon.ico
               ${NUM_COLLECT_HTML_ROOT}/favicon.ico COPYONLY)
