include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_library(num_collect ${SOURCE_FILES})
target_include_directories(
    num_collect
    PUBLIC $<BUILD_INTERFACE:${NUM_COLLECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${NUM_COLLECT_GENERATED_HEADER_DIR}>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(
    num_collect
    PUBLIC Threads::Threads
           fmt::fmt
           Eigen3::Eigen
           cpp_hash_tables::hash_tables
           tomlplusplus::tomlplusplus
           $<BUILD_INTERFACE:num_collect_cpp_warnings>
           $<BUILD_INTERFACE:num_collect_cpp_flags>)
target_compile_features(num_collect PUBLIC cxx_std_20)

set_target_properties(
    num_collect
    PROPERTIES C_VISIBILITY_PRESET hidden
               CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN 1
               DEBUG_POSTFIX d
               VERSION ${PROJECT_VERSION})

include(GenerateExportHeader)
generate_export_header(
    num_collect
    EXPORT_FILE_NAME
        "${NUM_COLLECT_GENERATED_HEADER_DIR}/num_collect/impl/num_collect_export.h"
)

add_library(num_collect_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_include_directories(
    num_collect_unity
    PUBLIC $<BUILD_INTERFACE:${NUM_COLLECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${NUM_COLLECT_GENERATED_HEADER_DIR}>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(
    num_collect_unity
    PUBLIC Threads::Threads
           fmt::fmt
           Eigen3::Eigen
           cpp_hash_tables::hash_tables
           tomlplusplus::tomlplusplus
           $<BUILD_INTERFACE:num_collect_cpp_warnings>
           $<BUILD_INTERFACE:num_collect_cpp_flags>)
target_compile_features(num_collect_unity PUBLIC cxx_std_20)
