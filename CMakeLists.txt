cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

# add options for testing
option(CODE_COVERAGE "Enable code coverage testing." OFF)
option(MEMORY_CHECK "Enable testing for memory leaks." OFF)
option(ENABLE_LOGGING "Enable internal library logging." OFF)
option(ENABLE_PROFILING "Enable valgrind profiling." OFF)
option(ENHANCED_PARAMETER_MAPPING "Use regex to map different parameter syntaxes." OFF)
option(ENABLE_BENCHMARKING "Benchmark with other database libraries." OFF)

# define project name
project (rj_db VERSION 0.5.0)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# check for libraries
find_package(MySQL)
find_package(SQLite3)
find_package(PostgreSQL)
find_package(Boost)

find_program(HEADERDOC headerdoc2html)
find_program(GATHERDOC gatherheaderdoc)
find_program(DOXYGEN doxygen)

# add target for code coverage
if(CODE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
	setup_target_for_coverage(TARGET ${PROJECT_NAME}_coverage OUTPUT ${PROJECT_BINARY_DIR}/gen/coverage)
endif()

if (ENABLE_LOGGING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING")
endif()

if (ENHANCED_PARAMETER_MAPPING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENHANCED_PARAMETER_MAPPING")
endif()

# add config header file
include(CreateConfigHeader)
create_config_header()

# add packge config file
include(CreatePackage)
create_package("A c++11 database interface.")

if (SQLITE3_FOUND)
	set(SQLITE_PROJECT_NAME "${PROJECT_NAME}_sqlite")
	set(SQLITE_TEST_PROJECT_NAME "${PROJECT_NAME}_sqlite_test")
endif()

if (MYSQL_VERSION)
	set(MYSQL_PROJECT_NAME "${PROJECT_NAME}_mysql")
	set(MYSQL_TEST_PROJECT_NAME "${PROJECT_NAME}_mysql_test")
endif()

if (PostgreSQL_FOUND)
	set(POSTGRES_PROJECT_NAME "${PROJECT_NAME}_postgres")
	set(POSTGRES_TEST_PROJECT_NAME "${PROJECT_NAME}_postgres_test")
endif()

set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")
set(UTIL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_util")

# add directories
add_subdirectory(src)
add_subdirectory(tests)

# Setup testing
enable_testing()

include(ValgrindTest)

add_valgrind_test(MEMORY_CHECK TARGET ${TEST_PROJECT_NAME})

if (MYSQL_VERSION)
	add_valgrind_test(MEMORY_CHECK TARGET ${MYSQL_TEST_PROJECT_NAME} VALGRIND_ARGS "--suppressions=${PROJECT_SOURCE_DIR}/suppression.map")
endif()

if (PostgreSQL_FOUND)
	add_valgrind_test(MEMORY_CHECK TARGET ${POSTGRES_TEST_PROJECT_NAME} VALGRIND_ARGS "--suppressions=${PROJECT_SOURCE_DIR}/suppression.map")
endif()

if (SQLITE3_FOUND)
	add_valgrind_test(MEMORY_CHECK TARGET ${SQLITE_TEST_PROJECT_NAME})
endif()

if (DOXYGEN)
	add_custom_target(html WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${DOXYGEN} ${PROJECT_SOURCE_DIR}/Doxyfile)
elseif (HEADERDOC)
	add_custom_target(html COMMAND ${HEADERDOC} -o ${PROJECT_BINARY_DIR}/html ${CMAKE_SOURCE_DIR}/src/**.h
																 COMMAND ${GATHERDOC} ${PROJECT_BINARY_DIR}/html)
endif()
