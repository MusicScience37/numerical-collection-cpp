if(NUM_COLLECT_BUILD_DOC)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v1/config_schema.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v1/config_schema.json COPYONLY)
endif()

if(NUM_COLLECT_TESTING)
    add_test(
        NAME num_collect_test_config_schema
        COMMAND
            ${PIPENV_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
            "--junit-xml=${NUM_COLLECT_JUNIT_DIR}/num_collect_test_config_schema.xml"
        WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
endif()
