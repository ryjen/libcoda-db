add_library (${PROJECT_NAME}
	alloc.cpp
  	bind_mapping.cpp
	bindable.cpp
	column.cpp
	delete_query.cpp
	insert_query.cpp
	join_clause.cpp
	log.cpp
	modify_query.cpp
	query.cpp
	record.cpp
	resultset.cpp
	row.cpp
	schema.cpp
	schema_factory.cpp
	select_query.cpp
  	session.cpp
	sqldb.cpp
	sql_common.cpp
	sql_number.cpp
	sql_time.cpp
	sql_value.cpp
  	transaction.cpp
	update_query.cpp
  	uri.cpp
	where_clause.cpp
	mysql/binding.cpp
	mysql/column.cpp
	mysql/resultset.cpp
	mysql/row.cpp
  	mysql/session.cpp
	mysql/statement.cpp
  	mysql/transaction.cpp
	postgres/binding.cpp
	postgres/column.cpp
	postgres/resultset.cpp
	postgres/row.cpp
  	postgres/session.cpp
	postgres/statement.cpp
  	postgres/transaction.cpp
	sqlite/column.cpp
	sqlite/resultset.cpp
	sqlite/row.cpp
  	sqlite/session.cpp
	sqlite/statement.cpp
 	sqlite/transaction.cpp
)

include_directories(${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR} ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES} ${SQLITE3_LIBRARIES} ${PostgreSQL_LIBRARIES})

set(${PROJECT_NAME}_HEADERS
  	bind_mapping.h
	bindable.h
	column.h
	delete_query.h
	exception.h
	insert_query.h
  	join_clause.h
	modify_query.h
	query.h
  	record.h
	resultset.h
	row.h
	schema.h
	schema_factory.h
	select_query.h
  	session.h
  	sql_types.h
  	sql_common.h
  	sql_number.h
	sql_value.h
	sqldb.h
	statement.h
  	transaction.h
	update_query.h
  	uri.h
	where_clause.h
)

set(${PROJECT_NAME}_MYSQL_HEADERS
	mysql/binding.h
	mysql/column.h
	mysql/resultset.h
	mysql/row.h
  	mysql/session.h
	mysql/statement.h
  	mysql/transaction.h
)

set(${PROJECT_NAME}_POSTGRES_HEADERS
  postgres/binding.h
  postgres/column.h
  postgres/resultset.h
  postgres/row.h
  postgres/session.h
  postgres/statement.h
  postgres/transaction.h
)

set(${PROJECT_NAME}_SQLITE_HEADERS
	sqlite/column.h
	sqlite/resultset.h
	sqlite/row.h
  	sqlite/session.h
	sqlite/statement.h
  	sqlite/transaction.h
)

string(REPLACE "_" "/" INSTALL_DIRECTORY ${PROJECT_NAME})

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}")

install(FILES ${${PROJECT_NAME}_MYSQL_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}/mysql")

install(FILES ${${PROJECT_NAME}_POSTGRES_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}/postgres")

install(FILES ${${PROJECT_NAME}_SQLITE_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}/sqlite")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
