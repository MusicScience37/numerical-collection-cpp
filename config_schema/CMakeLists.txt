if(NUM_COLLECT_BUILD_DOC)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/v1/config_schema.json
        ${NUM_COLLECT_HTML_ROOT}/config_schema/v1/config_schema.json COPYONLY)
endif()

if(NUM_COLLECT_TESTING)
    # TODO
endif()
