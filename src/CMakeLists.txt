add_executable(compiler.c)

add_subdirectory(scanner)
add_subdirectory(ds)

target_link_libraries(compiler
        PRIVATE
        scanner
        ds)
