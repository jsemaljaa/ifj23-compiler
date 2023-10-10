# Define the source files for the 'compiler' target
set(SOURCE_FILES
        scanner.c
        str.c
        compiler.c
        )

# Add the 'compiler' target
add_executable(compiler ${SOURCE_FILES})

# Include directories
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link any additional libraries if needed
# target_link_libraries(compiler PUBLIC YourAdditionalLibraries)
