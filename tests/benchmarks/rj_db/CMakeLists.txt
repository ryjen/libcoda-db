
include_directories(${PROJECT_SOURCE_DIR}/tests)

add_executable(${PROJECT_NAME}_benchmark 
	benchmark.cpp
	insert.cpp
	select.cpp
)

target_link_libraries (${PROJECT_NAME}_benchmark ${SQLITE_PROJECT_NAME} ${MYSQL_PROJECT_NAME} ${POSTGRES_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME})
