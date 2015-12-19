

link_directories("${PROJECT_BINARY_DIR}/libs/variant/src")

add_executable (${PROJECT_NAME}-test-mysql arg3.test.cpp delete_query.test.cpp mysql_db.test.cpp resultset.test.cpp select_query.test.cpp base_record.test.cpp modify_query.test.cpp mysql_resultset.test.cpp row.test.cpp sql_value.test.cpp column.test.cpp mysql_binding.test.cpp mysql_row.test.cpp schema.test.cpp db.test.cpp mysql_column.test.cpp mysql_statement.test.cpp schema_factory.test.cpp where_clause.test.cpp)

set_target_properties(${PROJECT_NAME}-test-mysql PROPERTIES COMPILE_FLAGS "-DTEST_MYSQL") 

add_executable (${PROJECT_NAME}-test-sqlite arg3.test.cpp delete_query.test.cpp resultset.test.cpp select_query.test.cpp base_record.test.cpp modify_query.test.cpp row.test.cpp sql_value.test.cpp column.test.cpp schema.test.cpp sqlite3_statement.test.cpp db.test.cpp schema_factory.test.cpp where_clause.test.cpp sqlite3_statement.test.cpp)

set_target_properties(${PROJECT_NAME}-test-sqlite PROPERTIES COMPILE_FLAGS "-DTEST_SQLITE") 

include_directories(SYSTEM vendor/bandit ${PROJECT_SOURCE_DIR}/libs/variant/src ${PROJECT_SOURCE_DIR}/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})

target_link_libraries (${PROJECT_NAME}-test-mysql ${PROJECT_NAME} arg3variant)

target_link_libraries (${PROJECT_NAME}-test-sqlite ${PROJECT_NAME} arg3variant)

add_executable (${PROJECT_NAME}-test run_all.test.cpp)

