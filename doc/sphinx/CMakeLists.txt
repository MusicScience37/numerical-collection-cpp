find_package(Doxygen REQUIRED)

# range of documentation
set(DOXYGEN_EXTRACT_ALL NO)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
set(DOXYGEN_EXTRACT_STATIC YES)
set(DOXYGEN_INTERNAL_DOCS YES)

# inputs
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_FILE_PATTERNS *.h)
set(DOXYGEN_INPUT_ENCODING UTF-8)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "")

# warnings
set(DOXYGEN_QUIET YES)
set(DOXYGEN_WARNINGS YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
set(DOXYGEN_WARN_NO_PARAMDOC YES)
set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS)

# for class documentations
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_SORT_BRIEF_DOCS YES)
set(DOXYGEN_SORT_MEMBER_DOCS YES)
set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)

# for preprocessors
set(DOXYGEN_ENABLE_PREPROCESSING YES)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
set(DOXYGEN_PREDEFINED NUM_COLLECT_DOCUMENTATION)
set(DOXYGEN_SEARCH_INCLUDES YES)
set(DOXYGEN_INCLUDE_PATH include)
set(DOXYGEN_STRIP_FROM_INC_PATH include)

# citation
set(DOXYGEN_CITE_BIB_FILES "doc/articles.bib")

# outputs
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(DOXYGEN_CREATE_SUBDIRS NO)

doxygen_add_docs(
    num_collect_doc_doxygen_xml "include" "doc/doxygen"
    WORKING_DIRECTORY "${NUM_COLLECT_SOURCE_DIR}"
    ALL
    COMMENT "Generate Doxygen XML API documentation for ${PROJECT_NAME}")

add_dependencies(num_collect_doc_doxygen_xml num_collect_doc_doxygen)

if(DEFINED ENV{PLANTUML_JAR_PATH})
    set(DEFAULT_PLANTUML_JAR_PATH $ENV{PLANTUML_JAR_PATH})
else()
    set(DEFAULT_PLANTUML_JAR_PATH "")
endif()
set(PLANTUML_JAR_PATH
    "${DEFAULT_PLANTUML_JAR_PATH}"
    CACHE FILEPATH "path of plantuml.jar")

set(NUM_COLLECT_DOXYGEN_XML_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen_xml")

set(NUM_COLLECT_SPHINX_OPTIONS -D "plantuml=java -jar ${PLANTUML_JAR_PATH}" -D
                               "release=${PROJECT_VERSION}")

add_custom_target(
    num_collect_doc_sphinx_html ALL
    COMMAND
        ${PIPENV_EXECUTABLE} run sphinx-build -b html -W
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${NUM_COLLECT_HTML_ROOT}
        ${NUM_COLLECT_SPHINX_OPTIONS} -q
    COMMENT "Generate Sphinx html API documentation for num_collect"
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})

add_dependencies(num_collect_doc num_collect_doc_sphinx_html)
add_dependencies(num_collect_doc_sphinx_html num_collect_doc_doxygen_xml)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/start_auto_build.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/start_auto_build.sh)
