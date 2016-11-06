
include_directories(${PROJECT_SOURCE_DIR}/tests)

add_executable(${PROJECT_NAME}_benchmark 
	benchmark.cpp
	insert.cpp
	select.cpp
)

target_link_libraries (${PROJECT_NAME}_benchmark ${PROJECT_NAME}-sqlite ${PROJECT_NAME}-mysql ${PROJECT_NAME}-postgres ${PROJECT_NAME}-test-util)
