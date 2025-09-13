if(NUM_COLLECT_BUILD_DOC)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v1/config_schema4.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v1/config_schema4.json COPYONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v1/config_schema7.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v1/config_schema7.json COPYONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v1/config_schema7.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v1/config_schema.json COPYONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v2/config_schema.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v2/config_schema.json COPYONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v3/config_schema.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v3/config_schema.json COPYONLY)
endif()

if(NUM_COLLECT_TESTING)
    add_test(
        NAME test_config_schema
        COMMAND ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
                "--junit-xml=${NUM_COLLECT_JUNIT_DIR}/test_config_schema.xml"
        WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
endif()
