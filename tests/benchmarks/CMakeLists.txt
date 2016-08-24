
add_subdirectory(rj_db)
add_subdirectory(poco)
add_subdirectory(soci)

add_custom_target(benchmark
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/rj_db/rj_db_benchmark
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/poco/rj_db_benchmark_poco
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/soci/rj_db_benchmark_soci
        COMMENT "Executing benchmarks...")
