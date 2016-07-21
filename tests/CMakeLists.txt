

link_directories("${PROJECT_BINARY_DIR}/libs/variant/src")

set(TEST_SOURCES
	main.db.test.cpp
	db.test.cpp
	column.test.cpp
	delete_query.test.cpp
	join_clause.test.cpp
	modify_query.test.cpp
	record.test.cpp
	resultset.test.cpp
	row.test.cpp
	schema.test.cpp
	schema_factory.test.cpp
	select_query.test.cpp
	transaction.test.cpp
	update_query.test.cpp
)

add_library(testicle
	testicle.cpp
)

add_executable (${PROJECT_NAME}_test_mysql
	${TEST_SOURCES}
	mysql/binding.test.cpp
	mysql/column.test.cpp
	mysql/resultset.test.cpp
	mysql/row.test.cpp
	mysql/session.test.cpp
	mysql/statement.test.cpp
)

set_target_properties(${PROJECT_NAME}_test_mysql PROPERTIES COMPILE_FLAGS "-DTEST_MYSQL")

add_executable (${PROJECT_NAME}_test_sqlite
	${TEST_SOURCES}
	sqlite/column.test.cpp
	sqlite/resultset.test.cpp
	sqlite/row.test.cpp
	sqlite/statement.test.cpp
	sqlite/transaction.test.cpp
)

set_target_properties(${PROJECT_NAME}_test_sqlite PROPERTIES COMPILE_FLAGS "-DTEST_SQLITE")

add_executable (${PROJECT_NAME}_test_postgres
	${TEST_SOURCES}
	postgres/binding.test.cpp
	postgres/column.test.cpp
	postgres/resultset.test.cpp
	postgres/row.test.cpp
	postgres/session.test.cpp
	postgres/statement.test.cpp
	postgres/transaction.test.cpp
)

set_target_properties(${PROJECT_NAME}_test_postgres PROPERTIES COMPILE_FLAGS "-DTEST_POSTGRES")

add_executable (${PROJECT_NAME}_test
	log.test.cpp
	main.test.cpp
	sql_value.test.cpp
	where_clause.test.cpp
)

include_directories(SYSTEM vendor/bandit SYSTEM vendor/benchpress/src ${PROJECT_SOURCE_DIR}/libs/variant/src ${PROJECT_SOURCE_DIR}/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR} ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries (${PROJECT_NAME}_test_mysql ${PROJECT_NAME} testicle rj_variant)

target_link_libraries (${PROJECT_NAME}_test_sqlite ${PROJECT_NAME} testicle rj_variant)

target_link_libraries (${PROJECT_NAME}_test_postgres ${PROJECT_NAME} testicle rj_variant)

target_link_libraries (${PROJECT_NAME}_test ${PROJECT_NAME} testicle rj_variant)

add_executable (${PROJECT_NAME}_test_all run_all.test.cpp)

if(ENABLE_BENCHMARKING)
add_subdirectory(benchmarks)
endif()
