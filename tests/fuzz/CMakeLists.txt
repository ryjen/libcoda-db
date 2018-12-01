
add_executable(${PROJECT_NAME}_fuzz main.test.cpp)

target_include_directories(${PROJECT_NAME}_fuzz PUBLIC ${PROJECT_SOURCE_DIR}/src)

message("Testing ${MYSQL_PROJECT_NAME}...")

target_link_libraries(${PROJECT_NAME}_fuzz ${MYSQL_PROJECT_NAME} ${PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME})

add_custom_target(${TEST_PROJECT_NAME}_fuzz DEPENDS ${PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME}
        COMMAND afl-fuzz -d -T ${TEST_PROJECT_NAME}_fuzz -x ${CMAKE_CURRENT_SOURCE_DIR}/grammar/record.dict -i ${CMAKE_CURRENT_SOURCE_DIR}/inputs -o ${PROJECT_BINARY_DIR}/fuzz ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_fuzz)
