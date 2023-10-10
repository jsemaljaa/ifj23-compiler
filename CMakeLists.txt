cmake_minimum_required(VERSION 3.25.2)
project(compiler)

# defines targets and sources
add_subdirectory(src)

# enable testing and define tests
enable_testing()
add_subdirectory(tests)

set(CMAKE_CXX_STANDARD 17)
add_executable(compiler compiler.c)

# cmake_minimum_required(VERSION 3.25)
#project(compiler C)
#
#set(CMAKE_C_STANDARD 17)
#
#add_executable(compiler main.c)
