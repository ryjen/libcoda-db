
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
        session_factory.h
        sql_types.h
        sql_common.h
        sql_generator.h
        sql_number.h
        sql_value.h
        sqldb.h
        statement.h
        transaction.h
        update_query.h
        uri.h
        where_clause.h
)

set(LIBRARY_SOURCES ${${PROJECT_NAME}_HEADERS}
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
        sql_generator.cpp
        sql_number.cpp
        sql_time.cpp
        sql_value.cpp
        transaction.cpp
        update_query.cpp
        uri.cpp
        where_clause.cpp
        exception.cpp)

string(REPLACE "_" "/" INSTALL_DIRECTORY ${PROJECT_NAME})

add_library(${PROJECT_NAME}
        ${LIBRARY_SOURCES}
        )

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

create_package(TARGET ${PROJECT_NAME} DESCRIPTION "A c++ database interface.")

if (SQLITE3_FOUND)

    add_library(${SQLITE_PROJECT_NAME}
            sqlite/column.cpp
            sqlite/resultset.cpp
            sqlite/row.cpp
            sqlite/session.cpp
            sqlite/statement.cpp
            sqlite/transaction.cpp
            )

    target_include_directories(${SQLITE_PROJECT_NAME} SYSTEM PUBLIC ${SQLITE3_INCLUDE_DIR})

    target_link_libraries(${SQLITE_PROJECT_NAME} ${PROJECT_NAME} ${SQLITE3_LIBRARIES})

    set(${PROJECT_NAME}_SQLITE_HEADERS
            sqlite/column.h
            sqlite/resultset.h
            sqlite/row.h
            sqlite/session.h
            sqlite/statement.h
            sqlite/transaction.h
            )

    install(FILES ${${PROJECT_NAME}_SQLITE_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}/sqlite")


    create_package(TARGET ${SQLITE_PROJECT_NAME} DESCRIPTION "A c++ sqlite3 database interface.")

endif ()

if (MYSQL_FOUND)

    add_library(${MYSQL_PROJECT_NAME}
            mysql/binding.cpp
            mysql/column.cpp
            mysql/resultset.cpp
            mysql/row.cpp
            mysql/session.cpp
            mysql/statement.cpp
            mysql/transaction.cpp
            )

    target_include_directories(${MYSQL_PROJECT_NAME} SYSTEM PUBLIC ${MYSQL_INCLUDE_DIR})

    target_link_libraries(${MYSQL_PROJECT_NAME} ${PROJECT_NAME} ${MYSQL_LIBRARIES})

    set(${PROJECT_NAME}_MYSQL_HEADERS
            mysql/binding.h
            mysql/column.h
            mysql/resultset.h
            mysql/row.h
            mysql/session.h
            mysql/statement.h
            mysql/transaction.h
            )

    install(FILES ${${PROJECT_NAME}_MYSQL_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}/mysql")

    create_package(TARGET ${MYSQL_PROJECT_NAME} DESCRIPTION "A c++ mysql database interface.")

endif ()

if (PostgreSQL_FOUND)

    add_library(${POSTGRES_PROJECT_NAME}
            postgres/binding.cpp
            postgres/column.cpp
            postgres/resultset.cpp
            postgres/row.cpp
            postgres/session.cpp
            postgres/statement.cpp
            postgres/transaction.cpp
            )

    target_include_directories(${POSTGRES_PROJECT_NAME} SYSTEM PUBLIC ${PostgreSQL_INCLUDE_DIRS})

    target_link_libraries(${POSTGRES_PROJECT_NAME} ${PROJECT_NAME} ${PostgreSQL_LIBRARIES})

    set(${PROJECT_NAME}_POSTGRES_HEADERS
            postgres/binding.h
            postgres/column.h
            postgres/resultset.h
            postgres/row.h
            postgres/session.h
            postgres/statement.h
            postgres/transaction.h
            )

    install(FILES ${${PROJECT_NAME}_POSTGRES_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}/postgres")

    create_package(TARGET ${POSTGRES_PROJECT_NAME} DESCRIPTION "A c++ database interface.")

endif ()

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}")

install(TARGETS ${PROJECT_NAME} ${SQLITE_PROJECT_NAME} ${MYSQL_PROJECT_NAME} ${POSTGRES_PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
