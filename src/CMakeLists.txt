add_library (${PROJECT_NAME} column.cpp mysql_column.cpp mysql_statement.cpp schema.cpp sqldb.cpp sqlite3_row.cpp delete_query.cpp mysql_db.cpp query.cpp schema_factory.cpp sqlite3_column.cpp sqlite3_statement.cpp modify_query.cpp mysql_resultset.cpp resultset.cpp select_query.cpp sqlite3_db.cpp where_clause.cpp mysql_binding.cpp mysql_row.cpp row.cpp sql_value.cpp sqlite3_resultset.cpp log.cpp)

include_directories(${PROJECT_SOURCE_DIR}/libs/variant/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES} ${SQLITE3_LIBRARIES})


set(${PROJECT_NAME}_HEADERS
	base_record.h
	bindable.h
	column.h
	delete_query.h
	exception.h
	modify_query.h
	mysql_binding.h
	mysql_column.h
	mysql_db.h
	mysql_resultset.h
	mysql_row.h
	mysql_statement.h
	query.h
	resultset.h
	row.h
	schema.h
	schema_factory.h
	select_query.h
	sql_value.h
	sqldb.h
	sqlite3_column.h
	sqlite3_db.h
	sqlite3_resultset.h
	sqlite3_row.h
	sqlite3_statement.h
	statement.h
	where_clause.h
)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
