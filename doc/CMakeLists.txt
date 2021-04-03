message("Configure documentation of num_collect")

add_custom_target(num_collect_doc)

option(NUM_COLLECT_BUILD_DOC_DOXYGEN
       "build documentation of num_collect with Doxygen" ON)
if(NUM_COLLECT_BUILD_DOC_DOXYGEN)
    add_subdirectory(doxygen)
endif()

option(NUM_COLLECT_BUILD_DOC_SPHINX
       "build documentation of num_collect with Sphinx" ON)
if(NUM_COLLECT_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()
