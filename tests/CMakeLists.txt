
set(TEST_SOURCES
	main.db.test.cpp
	db.test.cpp
	column.test.cpp
	delete_query.test.cpp
	insert_query.test.cpp
	join_clause.test.cpp
	record.test.cpp
	resultset.test.cpp
	row.test.cpp
	schema.test.cpp
	schema_factory.test.cpp
	select_query.test.cpp
	transaction.test.cpp
	update_query.test.cpp
)

set(COMMON_TEST_PROJECT_NAME "${TEST_PROJECT_NAME}_common")

find_program(AFL_COMPILER NAMES "afl-clang++" "afl-clang" "afl-gcc"	)

add_library(${COMMON_TEST_PROJECT_NAME}
	${TEST_SOURCES}
)

target_include_directories(${COMMON_TEST_PROJECT_NAME} SYSTEM PUBLIC vendor/bandit PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_library(${UTIL_TEST_PROJECT_NAME}
	util.cpp
)

target_include_directories(${UTIL_TEST_PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/src)

if (MYSQL_FOUND)

add_executable (${MYSQL_TEST_PROJECT_NAME}
	mysql/main.test.cpp
	mysql/binding.test.cpp
	mysql/column.test.cpp
	mysql/resultset.test.cpp
	mysql/row.test.cpp
	mysql/session.test.cpp
	mysql/statement.test.cpp
)

target_link_libraries (${MYSQL_TEST_PROJECT_NAME} ${MYSQL_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

target_include_directories(${MYSQL_TEST_PROJECT_NAME} SYSTEM PUBLIC ${MYSQL_INCLUDE_DIR} SYSTEM PUBLIC vendor/bandit INTERFACE ${PROJECT_SOURCE_DIR}/src)

endif()

if (SQLITE3_FOUND)

add_executable (${SQLITE_TEST_PROJECT_NAME}
	sqlite/main.test.cpp
	sqlite/column.test.cpp
	sqlite/resultset.test.cpp
	sqlite/row.test.cpp
	sqlite/statement.test.cpp
	sqlite/transaction.test.cpp
)

target_link_libraries (${SQLITE_TEST_PROJECT_NAME} ${SQLITE_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

target_include_directories(${SQLITE_TEST_PROJECT_NAME} SYSTEM PUBLIC ${SQLITE3_INCLUDE_DIR} SYSTEM PUBLIC vendor/bandit INTERFACE ${PROJECT_SOURCE_DIR}/src)

endif()

if (PostgreSQL_FOUND)

add_executable (${POSTGRES_TEST_PROJECT_NAME}
	postgres/main.test.cpp
	postgres/binding.test.cpp
	postgres/column.test.cpp
	postgres/resultset.test.cpp
	postgres/row.test.cpp
	postgres/session.test.cpp
	postgres/statement.test.cpp
	postgres/transaction.test.cpp
)

target_link_libraries (${POSTGRES_TEST_PROJECT_NAME} ${POSTGRES_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

target_include_directories(${POSTGRES_TEST_PROJECT_NAME} SYSTEM PUBLIC ${PostgreSQL_INCLUDE_DIRS} SYSTEM PUBLIC vendor/bandit INTERFACE ${PROJECT_SOURCE_DIR}/src)

endif()

add_executable (${TEST_PROJECT_NAME}
	log.test.cpp
	main.test.cpp
	sql_value.test.cpp
	where_clause.test.cpp
	util.cpp
)

target_link_libraries (${TEST_PROJECT_NAME} ${PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME})

target_include_directories(${TEST_PROJECT_NAME} SYSTEM PUBLIC vendor/bandit INTERFACE ${PROJECT_SOURCE_DIR}/src)

add_executable (${PROJECT_NAME}_test_all run_all.test.cpp)

if(ENABLE_BENCHMARKING)
add_subdirectory(benchmarks)
endif()

if (AFL_COMPILER)
add_subdirectory(fuzz)
endif()
