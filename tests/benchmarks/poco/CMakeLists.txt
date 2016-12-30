

add_executable(${PROJECT_NAME}_benchmark_poco
	insert.cpp
	select.cpp
)

target_include_directories(${PROJECT_NAME}_benchmark_poco SYSTEM vendor/benchpress/src)

target_include_directories(${PROJECT_NAME}_benchmark_poco ${PROJECT_SOURCE_DIR}/tests)

target_link_libraries (${PROJECT_NAME}_benchmark_poco PocoData PocoDataSqlite PocoFoundation ${UTIL_TEST_PROJECT_NAME})
