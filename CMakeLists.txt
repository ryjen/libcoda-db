cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

# add options for testing
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)
option(ENABLE_PROFILING "Enable profiling code usage." OFF)
option(ENABLE_PARAMETER_MAPPING "Use regex to map different parameter syntaxes." OFF)
option(ENABLE_BENCHMARKING "Benchmark with other database libraries." OFF)

# define project name
project (coda_db VERSION 0.5.1)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# check for libraries
find_package(MySQL)
find_package(SQLite3)
find_package(Postgres)

find_program(CPPCLEAN_FOUND cppclean)

if (ENABLE_PARAMETER_MAPPING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_PARAMETER_MAPPING")
endif()

if (SQLITE3_FOUND)
	set(SQLITE_PROJECT_NAME "${PROJECT_NAME}_sqlite")
endif()

if (MYSQL_FOUND)
	set(MYSQL_PROJECT_NAME "${PROJECT_NAME}_mysql")
endif()

if (POSTGRES_FOUND)
	set(POSTGRES_PROJECT_NAME "${PROJECT_NAME}_postgres")
endif()

# add packge config file
include(CreatePackage)
include(CreateDocumentation)

create_documentation()

# Setup testing
enable_testing()

# add directories
add_subdirectory(src)
add_subdirectory(tests)

if (CPPCLEAN_FOUND)
	add_custom_target(cppclean COMMAND ${CPPCLEAN_FOUND} ${PROJECT_SOURCE_DIR}/src)
endif()

############################################################
# Create DEB
############################################################

# Tell CPack to generate a .deb package
set(CPACK_GENERATOR "DEB")

# Set a Package Maintainer.
# This is required
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ryan Jennings <ryan@micrantha.com>")

set(CPACK_PACKAGE_VENDOR "Micrantha Software Solutions")

# Set a Package Version
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A c++ toolkit")

set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Include CPack
include(CPack)

