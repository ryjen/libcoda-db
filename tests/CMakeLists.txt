add_executable (${PROJECT_NAME}_test arg3.test.cpp delete_query.test.cpp mysql_db.test.cpp resultset.test.cpp select_query.test.cpp base_record.test.cpp modify_query.test.cpp mysql_resultset.test.cpp row.test.cpp sql_value.test.cpp column.test.cpp mysql_binding.test.cpp mysql_row.test.cpp schema.test.cpp sqlite3_statement.test.cpp db.test.cpp mysql_column.test.cpp mysql_statement.test.cpp schema_factory.test.cpp where_clause.test.cpp sqlite3_statement.test.cpp)

include_directories(SYSTEM vendor/bandit ${PROJECT_SOURCE_DIR}/libs/variant/src ${PROJECT_SOURCE_DIR}/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})

target_link_libraries (${PROJECT_NAME}_test ${PROJECT_NAME} arg3variant)

