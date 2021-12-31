find_package(Doxygen REQUIRED dot)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

file(MAKE_DIRECTORY ${NUM_COLLECT_HTML_ROOT}/api)

add_custom_target(
    num_collect_doc_doxygen ALL
    COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generate Doxygen HTML API documentation for ${FULL_PROJECT_NAME}"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})

add_dependencies(num_collect_doc num_collect_doc_doxygen)
