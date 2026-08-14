cmake_minimum_required(VERSION 3.21)

project(coda_db VERSION 0.5.1 LANGUAGES CXX)

# build/test options
option(CODA_BUILD_TESTS "Build libcoda-db tests." ON)
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)
option(ENABLE_PROFILING "Enable profiling code usage." OFF)
option(ENABLE_PARAMETER_MAPPING "Use regex to map different parameter syntaxes." OFF)
option(ENABLE_BENCHMARKING "Benchmark with other database libraries." OFF)
option(ENABLE_TEST_DB_FIXTURES "Start disposable local databases for CTest integration targets." ${UNIX})

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# check for libraries
find_package(MySQL)
find_package(SQLite3)
find_package(PostgreSQL)

find_program(CPPCLEAN_FOUND cppclean)

# Coverage still uses the legacy shared helper's global flags. Moving coverage
# instrumentation to target-local options is a separate issue #2/#5 slice.
if(ENABLE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
	setup_target_for_coverage(TARGET ${PROJECT_NAME}_coverage OUTPUT ${PROJECT_BINARY_DIR}/gen/coverage)
endif()

include(CreateDocumentation)
create_documentation()

if (SQLITE3_FOUND)
	set(SQLITE_PROJECT_NAME "${PROJECT_NAME}_sqlite")
	set(SQLITE_TEST_PROJECT_NAME "${PROJECT_NAME}_test_sqlite")
endif()

if (MYSQL_FOUND)
	set(MYSQL_PROJECT_NAME "${PROJECT_NAME}_mysql")
	set(MYSQL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_mysql")
endif()

if (PostgreSQL_FOUND)
	set(POSTGRES_PROJECT_NAME "${PROJECT_NAME}_postgres")
	set(POSTGRES_TEST_PROJECT_NAME "${PROJECT_NAME}_test_postgres")
endif()

set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")
set(UTIL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_util")

add_subdirectory(src)

if(CODA_BUILD_TESTS)
	add_subdirectory(tests)
	enable_testing()

	include(ValgrindTest)
	add_valgrind_profile_test(ENABLE_PROFILING ENABLE_MEMCHECK TARGET ${TEST_PROJECT_NAME})

	if (MYSQL_FOUND)
		add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${MYSQL_TEST_PROJECT_NAME} )
	endif()

	if (PostgreSQL_FOUND)
		add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${POSTGRES_TEST_PROJECT_NAME} )
	endif()

	if (SQLITE3_FOUND)
		add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${SQLITE_TEST_PROJECT_NAME})
	endif()

	if (ENABLE_TEST_DB_FIXTURES AND UNIX)
		find_program(BASH_COMMAND bash REQUIRED)

		if (MYSQL_FOUND)
			set(MYSQL_FIXTURE_DIR "${PROJECT_BINARY_DIR}/test-fixtures/mysql")
			add_test(NAME ${MYSQL_TEST_PROJECT_NAME}_setup
				COMMAND ${BASH_COMMAND} ${PROJECT_SOURCE_DIR}/tests/fixtures/mysql-fixture.sh setup ${MYSQL_FIXTURE_DIR})
			add_test(NAME ${MYSQL_TEST_PROJECT_NAME}_cleanup
				COMMAND ${BASH_COMMAND} ${PROJECT_SOURCE_DIR}/tests/fixtures/mysql-fixture.sh cleanup ${MYSQL_FIXTURE_DIR})
			set_tests_properties(${MYSQL_TEST_PROJECT_NAME}_setup PROPERTIES FIXTURES_SETUP mysql)
			set_tests_properties(${MYSQL_TEST_PROJECT_NAME}_cleanup PROPERTIES FIXTURES_CLEANUP mysql)
			set_tests_properties(${MYSQL_TEST_PROJECT_NAME} PROPERTIES
				FIXTURES_REQUIRED mysql
				ENVIRONMENT "MYSQL_URI=mysql://root@127.0.0.1:53306/test")
		endif()

		if (PostgreSQL_FOUND)
			set(POSTGRES_FIXTURE_DIR "${PROJECT_BINARY_DIR}/test-fixtures/postgres")
			add_test(NAME ${POSTGRES_TEST_PROJECT_NAME}_setup
				COMMAND ${BASH_COMMAND} ${PROJECT_SOURCE_DIR}/tests/fixtures/postgres-fixture.sh setup ${POSTGRES_FIXTURE_DIR})
			add_test(NAME ${POSTGRES_TEST_PROJECT_NAME}_cleanup
				COMMAND ${BASH_COMMAND} ${PROJECT_SOURCE_DIR}/tests/fixtures/postgres-fixture.sh cleanup ${POSTGRES_FIXTURE_DIR})
			set_tests_properties(${POSTGRES_TEST_PROJECT_NAME}_setup PROPERTIES FIXTURES_SETUP postgres)
			set_tests_properties(${POSTGRES_TEST_PROJECT_NAME}_cleanup PROPERTIES FIXTURES_CLEANUP postgres)
			set_tests_properties(${POSTGRES_TEST_PROJECT_NAME} PROPERTIES
				FIXTURES_REQUIRED postgres
				ENVIRONMENT "POSTGRES_URI=postgres://127.0.0.1:55432/test")
		endif()
	endif()
endif()

if (CPPCLEAN_FOUND)
	add_custom_target(cppclean COMMAND ${CPPCLEAN_FOUND} ${PROJECT_SOURCE_DIR}/src)
endif()
