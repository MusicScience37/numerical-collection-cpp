if(DEFINED ENV{PLANTUML_JAR_PATH})
    set(DEFAULT_PLANTUML_JAR_PATH $ENV{PLANTUML_JAR_PATH})
else()
    set(DEFAULT_PLANTUML_JAR_PATH "")
endif()
set(PLANTUML_JAR_PATH
    "${DEFAULT_PLANTUML_JAR_PATH}"
    CACHE FILEPATH "path of plantuml.jar")

set(NUM_COLLECT_SPHINX_OPTIONS -D "plantuml=java -jar ${PLANTUML_JAR_PATH}" -D
                               "release=${PROJECT_VERSION}")

add_custom_target(
    num_collect_doc_sphinx_html ALL
    COMMAND
        ${POETRY_EXECUTABLE} run sphinx-build -b html -W
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${NUM_COLLECT_HTML_ROOT}
        ${NUM_COLLECT_SPHINX_OPTIONS} -q
    COMMENT "Generate Sphinx html API documentation for ${FULL_PROJECT_NAME}"
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})

add_dependencies(num_collect_doc num_collect_doc_sphinx_html)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/start_auto_build.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/start_auto_build.sh)
