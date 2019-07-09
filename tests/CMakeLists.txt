

if (NOT DEFINED BANDIT_DIR)

  include(ExternalProject)

  set(BANDIT_TARGET ${PROJECT_NAME}-bandit)

  externalproject_add(${BANDIT_TARGET}
    GIT_REPOSITORY https://github.com/banditcpp/bandit.git
    GIT_TAG v2.0.0
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/bandit)

  ExternalProject_Get_property(${BANDIT_TARGET} SOURCE_DIR)

  set(BANDIT_DIR ${SOURCE_DIR})

endif ()

set(TEST_SOURCES
  main.db.test.cpp
  db.test.cpp
  db.test.h
  column.test.cpp
  delete_query.test.cpp
  insert_query.test.cpp
  join_clause.test.cpp
  record.test.cpp
  resultset.test.cpp
  row.test.cpp
  schema.test.cpp
  schema_factory.test.cpp
  select_query.test.cpp
  transaction.test.cpp
  update_query.test.cpp
  )

set(COMMON_TEST_PROJECT_NAME "${TEST_PROJECT_NAME}_common")

if (SQLITE3_FOUND)
  set(SQLITE_TEST_PROJECT_NAME "${PROJECT_NAME}_test_sqlite")
endif ()

if (MYSQL_FOUND)
  set(MYSQL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_mysql")
endif ()

if (POSTGRES_FOUND)
  set(POSTGRES_TEST_PROJECT_NAME "${PROJECT_NAME}_test_postgres")
endif ()

set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")
set(UTIL_TEST_PROJECT_NAME "${PROJECT_NAME}_test_util")

find_program(AFL_COMPILER NAMES "afl-clang++" "afl-clang" "afl-gcc")

add_library(${COMMON_TEST_PROJECT_NAME}
  ${TEST_SOURCES}
  )

target_include_directories(${COMMON_TEST_PROJECT_NAME} SYSTEM PUBLIC ${BANDIT_DIR}
  PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_library(${UTIL_TEST_PROJECT_NAME}
  util.cpp
  )

target_include_directories(${UTIL_TEST_PROJECT_NAME} SYSTEM PUBLIC ${BANDIT_DIR} INTERFACE ${PROJECT_SOURCE_DIR}/src)

if (POSTGRES_FOUND)

  add_executable(${POSTGRES_TEST_PROJECT_NAME}
    postgres/main.test.cpp
    postgres/binding.test.cpp
    postgres/column.test.cpp
    postgres/resultset.test.cpp
    postgres/row.test.cpp
    postgres/session.test.cpp
    postgres/statement.test.cpp
    postgres/transaction.test.cpp
    )

  target_link_libraries(${POSTGRES_TEST_PROJECT_NAME} ${POSTGRES_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

  target_include_directories(${POSTGRES_TEST_PROJECT_NAME} SYSTEM PUBLIC ${POSTGRES_INCLUDE_DIRS} SYSTEM PUBLIC ${BANDIT_DIR} INTERFACE ${PROJECT_SOURCE_DIR}/src)

endif ()

if (MYSQL_FOUND)

  add_executable(${MYSQL_TEST_PROJECT_NAME}
    mysql/main.test.cpp
    mysql/binding.test.cpp
    mysql/column.test.cpp
    mysql/resultset.test.cpp
    mysql/row.test.cpp
    mysql/session.test.cpp
    mysql/statement.test.cpp
    )

  target_link_libraries(${MYSQL_TEST_PROJECT_NAME} ${MYSQL_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

  target_link_libraries(${MYSQL_TEST_PROJECT_NAME} ${MYSQL_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

  link_directories (${MYSQL_TEST_PROJECT_NAME} PUBLIC ${MYSQL_LIB_DIR})

  target_include_directories(${MYSQL_TEST_PROJECT_NAME} SYSTEM PUBLIC ${MYSQL_INCLUDE_DIR} SYSTEM PUBLIC ${BANDIT_DIR} INTERFACE ${PROJECT_SOURCE_DIR}/src)

endif ()

if (SQLITE3_FOUND)

  add_executable(${SQLITE_TEST_PROJECT_NAME}
    sqlite/main.test.cpp
    sqlite/column.test.cpp
    sqlite/resultset.test.cpp
    sqlite/row.test.cpp
    sqlite/statement.test.cpp
    sqlite/transaction.test.cpp
    )

  target_link_libraries(${SQLITE_TEST_PROJECT_NAME} ${SQLITE_PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME} ${COMMON_TEST_PROJECT_NAME})

  target_include_directories(${SQLITE_TEST_PROJECT_NAME} SYSTEM PUBLIC ${SQLITE3_INCLUDE_DIR} SYSTEM PUBLIC ${BANDIT_DIR} INTERFACE ${PROJECT_SOURCE_DIR}/src)

endif ()

add_executable(${TEST_PROJECT_NAME}
  main.test.cpp
  sql_value.test.cpp
  where_clause.test.cpp
  util.cpp
  )

target_link_libraries(${TEST_PROJECT_NAME} ${PROJECT_NAME} ${UTIL_TEST_PROJECT_NAME})

target_include_directories(${TEST_PROJECT_NAME} SYSTEM PUBLIC ${BANDIT_DIR} INTERFACE ${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_test_all run_all.test.cpp)

if (ENABLE_BENCHMARKING)
  add_subdirectory(benchmarks)
endif ()

# add target for code coverage
if (ENABLE_COVERAGE)
  include(CodeCoverage)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}" CACHE STRING "coverage compile flags" FORCE)
  setup_target_for_coverage(TARGET ${PROJECT_NAME}_coverage OUTPUT ${PROJECT_BINARY_DIR}/gen/coverage)
endif ()

include(ValgrindTest)
add_valgrind_profile_test(ENABLE_PROFILING ENABLE_MEMCHECK TARGET ${TEST_PROJECT_NAME} EXECUTABLE_ARGS --reporter=spec)

if (MYSQL_FOUND)
  add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${MYSQL_TEST_PROJECT_NAME} EXECUTABLE_ARGS --reporter=spec)
endif ()

if (POSTGRES_FOUND)
  add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${POSTGRES_TEST_PROJECT_NAME} EXECUTABLE_ARGS --reporter=spec)
endif ()

if (SQLITE3_FOUND)
  add_valgrind_profile_test(PROFILING ${ENABLE_PROFILING} MEMCHECK ${ENABLE_MEMCHECK} TARGET ${SQLITE_TEST_PROJECT_NAME} EXECUTABLE_ARGS --reporter=spec)
endif ()

if (AFL_COMPILER)
  add_subdirectory(fuzz)
endif ()

if (DEFINED BANDIT_TARGET)
  add_dependencies(${COMMON_TEST_PROJECT_NAME} ${BANDIT_TARGET})

  if (MYSQL_FOUND)
    add_dependencies(${MYSQL_TEST_PROJECT_NAME} ${BANDIT_TARGET})
  endif ()

  if (SQLITE3_FOUND)
    add_dependencies(${SQLITE_TEST_PROJECT_NAME} ${BANDIT_TARGET})
  endif ()

  if (POSTGRES_FOUND)
    add_dependencies(${POSTGRES_TEST_PROJECT_NAME} ${BANDIT_TARGET})
  endif ()

  add_dependencies(${TEST_PROJECT_NAME} ${BANDIT_TARGET})
endif ()