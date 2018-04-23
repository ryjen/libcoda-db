
add_subdirectory(coda_db)
add_subdirectory(poco)
add_subdirectory(soci)

add_custom_target(benchmark
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/coda_db/coda_db_benchmark
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/poco/coda_db_benchmark_poco
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/soci/coda_db_benchmark_soci
        COMMENT "Executing benchmarks...")
