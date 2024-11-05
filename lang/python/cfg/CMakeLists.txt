# Load this to build the sourcepp Python package

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)
project(sourcepp_python)

if (NOT SKBUILD)
    message(WARNING "\
This CMake file is meant to be executed using 'scikit-build-core'.
Running it directly will almost certainly not produce the desired
result. If you are a user trying to install this package, use the
command below, which will install all necessary build dependencies,
compile the package in an isolated environment, and then install it.
=====================================================================
 $ pip install .
=====================================================================
If you are a software developer, and this is your own package, then
it is usually much more efficient to install the build dependencies
in your environment once and use the following command that avoids
a costly creation of a new virtual environment at every compilation:
=====================================================================
 $ pip install nanobind scikit-build-core[pyproject]
 $ pip install --no-build-isolation -ve .
=====================================================================
You may optionally add -Ceditable.rebuild=true to auto-rebuild when
the package is imported. Otherwise, you need to rerun the above
after editing C++ files.")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# As weird as this looks, this is necessary for sdist wheel
include(FetchContent)
FetchContent_Declare(
        sourcepp
        GIT_REPOSITORY "https://github.com/craftablescience/sourcepp.git"
        GIT_TAG "@SOURCEPP_GIT_TAG@")
set(SOURCEPP_BUILD_PYTHON_WRAPPERS ON CACHE INTERNAL "" FORCE)
set(SOURCEPP_PYTHON_VERSION "@SOURCEPP_PYTHON_VERSION@")
FetchContent_MakeAvailable(sourcepp)
