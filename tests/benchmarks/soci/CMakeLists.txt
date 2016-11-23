
include_directories(${PROJECT_SOURCE_DIR}/tests)

add_executable(${PROJECT_NAME}_benchmark_soci 
	insert.cpp
	select.cpp
)

target_link_libraries (${PROJECT_NAME}_benchmark_soci soci_core soci_sqlite3 soci_mysql soci_postgresql ${UTIL_TEST_PROJECT_NAME})
