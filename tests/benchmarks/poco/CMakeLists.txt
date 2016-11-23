
include_directories(${PROJECT_SOURCE_DIR}/tests)

add_executable(${PROJECT_NAME}_benchmark_poco
	insert.cpp
	select.cpp
)

target_link_libraries (${PROJECT_NAME}_benchmark_poco PocoData PocoDataSqlite PocoFoundation ${UTIL_TEST_PROJECT_NAME})
