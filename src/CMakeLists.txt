add_library (${PROJECT_NAME} column.cpp mysql_column.cpp mysql_statement.cpp schema.cpp sqldb.cpp sqlite3_row.cpp delete_query.cpp mysql_db.cpp query.cpp schema_factory.cpp sqlite3_column.cpp sqlite3_statement.cpp modify_query.cpp mysql_resultset.cpp resultset.cpp select_query.cpp sqlite3_db.cpp where_clause.cpp mysql_binding.cpp mysql_row.cpp row.cpp sql_value.cpp sqlite3_resultset.cpp log.cpp)

include_directories(${PROJECT_SOURCE_DIR}/libs/variant/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES} ${SQLITE3_LIBRARIES})
