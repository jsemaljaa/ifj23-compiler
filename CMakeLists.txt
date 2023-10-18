cmake_minimum_required(VERSION 3.22)
project(compiler C)
set(CMAKE_C_STANDARD 17)



# defines targets and sources
add_subdirectory(src)

# enable testing and define tests
enable_testing()
add_subdirectory(tests)





