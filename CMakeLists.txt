cmake_minimum_required(VERSION 3.15)
project(spatio_index)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find Python and pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Source files
set(SOURCE_FILES
    src/record_store.cpp
    src/spatial_index.cpp
    src/temporal_index.cpp
    src/spatio_index_core.cpp
)

# Create Python module
pybind11_add_module(_spatio_core 
    ${SOURCE_FILES}
    src/bindings.cpp
)

# Set output directory for the Python module
set_target_properties(_spatio_core PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/spatio
)

# Enable optimizations for release builds
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(_spatio_core PRIVATE /O2 /W4)
else()
    target_compile_options(_spatio_core PRIVATE -O3 -Wall -Wextra)
endif()
