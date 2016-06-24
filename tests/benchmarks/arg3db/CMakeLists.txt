
include_directories(${PROJECT_SOURCE_DIR}/tests)

file (GLOB_RECURSE source_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}-benchmark ${source_FILES} ${PROJECT_SOURCE_DIR}/tests/db.test.cpp)

target_link_libraries (${PROJECT_NAME}-benchmark ${PROJECT_NAME} ${PROJECT_NAME}-testlib arg3variant)
