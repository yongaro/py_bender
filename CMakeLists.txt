# Minimal version is 3.1 for policy CMP0054
cmake_minimum_required(VERSION 3.15)
project(PyBender)

# Simply add this project using add_subdirectory().
# Either include the PYBENDER_INCLUDE_DIRS or the Python3_INCLUDE_DIRS and the py_bender/include dir.
# Finally add the Python3_LIBRARIES to the target_link_libraries()
# By default the dynamic version of Python is used and the binaries are automatically copied next the binary executable.


if(CMAKE_CXX_STANDARD STRLESS "17")
    message(FATAL_ERROR "C++ 17 is required for the template systems that use the unfolding expressions.")
endif()

# Configuring Python
find_package(PythonLibs 3 REQUIRED)
find_package(Python3 COMPONENTS Development)
set(PYBENDER_INCLUDE_DIRS "${Python3_INCLUDE_DIRS}")
list(APPEND PYBENDER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(COPY "${Python3_LIBRARIES}" DESTINATION "${CMAKE_BINARY_DIR}")
