add_library (${PROJECT_NAME}
	alloc.cpp
	bindable.cpp
	column.cpp
	join_clause.cpp
	log.cpp
	modify_query.cpp
	query.cpp
	resultset.cpp
	row.cpp
	schema.cpp
	schema_factory.cpp
	select_query.cpp
	sqldb.cpp
	sql_value.cpp
	where_clause.cpp
	mysql/binding.cpp
	mysql/column.cpp
	mysql/db.cpp
	mysql/resultset.cpp
	mysql/row.cpp
	mysql/statement.cpp
	postgres/binding.cpp
	postgres/column.cpp
	postgres/db.cpp
	postgres/resultset.cpp
	postgres/row.cpp
	postgres/statement.cpp
	sqlite/column.cpp
	sqlite/db.cpp
	sqlite/resultset.cpp
	sqlite/row.cpp
	sqlite/statement.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/libs/variant/src ${MYSQL_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR} ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES} ${SQLITE3_LIBRARIES} ${PostgreSQL_LIBRARIES})

set(${PROJECT_NAME}_HEADERS
	base_record.h
	bindable.h
	column.h
	exception.h
  	join_clause.h
	modify_query.h
	query.h
	resultset.h
	row.h
	schema.h
	schema_factory.h
	select_query.h
	sql_value.h
	sqldb.h
	statement.h
	where_clause.h
)

set(${PROJECT_NAME}_MYSQL_HEADERS
	mysql/binding.h
	mysql/column.h
	mysql/db.h
	mysql/resultset.h
	mysql/row.h
	mysql/statement.h
)

set(${PROJECT_NAME}_POSTGRES_HEADERS
  postgres/db.h
  postgres/statement.h
  postgres/resultset.h
  postgres/row.h
  postgres/column.h
  postgres/binding.h
)

set(${PROJECT_NAME}_SQLITE_HEADERS
	sqlite3/column.h
	sqlite3/db.h
	sqlite3/resultset.h
	sqlite3/row.h
	sqlite3/statement.h
)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}")

install(FILES ${${PROJECT_NAME}_MYSQL_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}/mysql")

install(FILES ${${PROJECT_NAME}_POSTGRES_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}/postgres")

install(FILES ${${PROJECT_NAME}_SQLITE_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}/sqlite")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
