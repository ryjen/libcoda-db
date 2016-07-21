
include_directories(${PROJECT_SOURCE_DIR}/tests)

file (GLOB_RECURSE source_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_benchmark_soci ${source_FILES})

target_link_libraries (${PROJECT_NAME}_benchmark_soci testicle soci_core soci_sqlite3)
