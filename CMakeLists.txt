cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

# add options for testing
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)
option(ENABLE_LOGGING "Enable internal library logging." OFF)
option(ENABLE_PROFILING "Enable valgrind profiling." OFF)
option(ENABLE_PARAMETER_MAPPING "Use regex to map different parameter syntaxes." OFF)
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

# add target for code coverage
if(ENABLE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
	setup_target_for_coverage(TARGET ${PROJECT_NAME}_coverage OUTPUT ${PROJECT_BINARY_DIR}/gen/coverage)
endif()

if (ENABLE_LOGGING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING")
endif()

if (ENABLE_PARAMETER_MAPPING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_PARAMETER_MAPPING")
endif()

# add packge config file
include(CreatePackage)
create_package("A c++11 database interface.")

include(CreateDocumentation)
create_documentation()

if (SQLITE3_FOUND)
	set(SQLITE_PROJECT_NAME "${PROJECT_NAME}_sqlite")
	set(SQLITE_TEST_PROJECT_NAME "${PROJECT_NAME}_test_sqlite")
endif()

if (MYSQL_VERSION)
	set(MYSQL_PROJECT_NAME "${PROJECT_NAME}_mysql")
	set(MYSQL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_mysql")
endif()

if (PostgreSQL_FOUND)
	set(POSTGRES_PROJECT_NAME "${PROJECT_NAME}_postgres")
	set(POSTGRES_TEST_PROJECT_NAME "${PROJECT_NAME}_test_postgres")
endif()

set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")
set(UTIL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_util")

# add directories
add_subdirectory(src)
add_subdirectory(tests)

# Setup testing
enable_testing()

include(ValgrindTest)
add_valgrind_profile_test(ENABLE_PROFILING ENABLE_MEMCHECK TARGET ${TEST_PROJECT_NAME})

if (MYSQL_VERSION)
	add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${MYSQL_TEST_PROJECT_NAME} VALGRIND_ARGS "--suppressions=${PROJECT_SOURCE_DIR}/suppression.map")
endif()

if (PostgreSQL_FOUND)
	add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${POSTGRES_TEST_PROJECT_NAME} VALGRIND_ARGS "--suppressions=${PROJECT_SOURCE_DIR}/suppression.map")
endif()

if (SQLITE3_FOUND)
	add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${SQLITE_TEST_PROJECT_NAME})
endif()

