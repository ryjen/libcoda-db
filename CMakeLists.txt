cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

# add options for testing
option(CODE_COVERAGE "Enable code coverage testing." OFF)
option(MEMORY_CHECK "Enable testing for memory leaks." OFF)
option(ENABLE_LOGGING "Enable internal library logging." OFF)
option(ENABLE_PROFILING "Enable valgrind profiling." OFF)
option(ENHANCED_PARAMETER_MAPPING "Use regex to map different parameter syntaxes." ON)
option(ENABLE_BENCHMARKING "Benchmark with other database libraries." OFF)

# define project name
project (rj_db VERSION 0.5.0)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# check for libraries
find_package(MySQL)
find_package(SQLite3)
find_package(PostgreSQL)

find_program(HEADERDOC headerdoc2html)
find_program(GATHERDOC gatherheaderdoc)
find_program(DOXYGEN doxygen)

# add target for code coverage
if(CODE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
	setup_target_for_coverage(${PROJECT_NAME}_coverage ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}_test_all ${PROJECT_SOURCE_DIR}/coverage)
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

# add directories
add_subdirectory(libs/variant)
add_subdirectory(src)
add_subdirectory(tests)

# Setup testing
enable_testing()

include(ValgrindTest)

add_opt_valgrind_test(MEMORY_CHECK ${PROJECT_NAME}_test ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}-test)

if (MYSQL_FOUND)
	add_opt_valgrind_test(MEMORY_CHECK ${PROJECT_NAME}_test_mysql ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}-mysql-test)
endif()

if (PostgreSQL_FOUND)
	add_opt_valgrind_test(MEMORY_CHECK ${PROJECT_NAME}_test_postgres ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}-postgres-test)
endif()

if (SQLITE3_FOUND)
	add_opt_valgrind_test(MEMORY_CHECK ${PROJECT_NAME}_test_sqlite ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}-sqlite-test)
endif()

if (DOXYGEN)
	add_custom_target(html WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${DOXYGEN} ${PROJECT_SOURCE_DIR}/Doxyfile)
elseif (HEADERDOC)
	add_custom_target(html COMMAND ${HEADERDOC} -o ${PROJECT_BINARY_DIR}/html ${CMAKE_SOURCE_DIR}/src/**.h
																 COMMAND ${GATHERDOC} ${PROJECT_BINARY_DIR}/html)
endif()
