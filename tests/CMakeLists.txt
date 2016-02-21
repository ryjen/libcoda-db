

link_directories("${PROJECT_BINARY_DIR}/libs/variant/src")

set(TEST-SOURCES
  arg3.test.cpp
  column.test.cpp
  db.test.cpp
  delete_query.test.cpp
  join.test.cpp
  modify_query.test.cpp
  record.test.cpp
  resultset.test.cpp
  row.test.cpp
  schema.test.cpp
  schema_factory.test.cpp
  select_query.test.cpp
  sql_value.test.cpp
  where_clause.test.cpp
)

add_executable (${PROJECT_NAME}-test-mysql
	${TEST-SOURCES}
	mysql_binding.test.cpp
	mysql_column.test.cpp
	mysql_db.test.cpp
	mysql_resultset.test.cpp
	mysql_row.test.cpp
	mysql_statement.test.cpp
)

set_target_properties(${PROJECT_NAME}-test-mysql PROPERTIES COMPILE_FLAGS "-DTEST_MYSQL")

add_executable (${PROJECT_NAME}-test-sqlite
  ${TEST-SOURCES}
  sqlite3_column.test.cpp
  sqlite3_statement.test.cpp
)

set_target_properties(${PROJECT_NAME}-test-sqlite PROPERTIES COMPILE_FLAGS "-DTEST_SQLITE")

add_executable (${PROJECT_NAME}-test-postgres
  ${TEST-SOURCES}
  postgres_column.test.cpp
  postgres_db.test.cpp
  postgres_binding.test.cpp
  postgres_statement.test.cpp
)

set_target_properties(${PROJECT_NAME}-test-postgres PROPERTIES COMPILE_FLAGS "-DTEST_POSTGRES")

include_directories(SYSTEM vendor/bandit ${PROJECT_SOURCE_DIR}/libs/variant/src ${PROJECT_SOURCE_DIR}/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR} ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries (${PROJECT_NAME}-test-mysql ${PROJECT_NAME} arg3variant)

target_link_libraries (${PROJECT_NAME}-test-sqlite ${PROJECT_NAME} arg3variant)

target_link_libraries (${PROJECT_NAME}-test-postgres ${PROJECT_NAME} arg3variant)

add_executable (${PROJECT_NAME}-test run_all.test.cpp)
