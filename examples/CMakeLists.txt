add_subdirectory(util)
add_subdirectory(opt)
add_subdirectory(roots)
add_subdirectory(ode)
add_subdirectory(auto_diff)

find_program(
    PYTHON_EXECUTABLE
    NAMES python3 python
    PATHS ${PIPENV_VENV_DIR}/bin ${PIPENV_VENV_DIR}/Scripts
    NO_DEFAULT_PATH)
if(NOT PYTHON_EXECUTABLE)
    message(STATUS "Python not found for Python interface")
    return()
endif()
message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")

find_package(pybind11 REQUIRED)

add_subdirectory(regularization)
