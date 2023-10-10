add_library(scanner "")

target_sources(scanner
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/scanner.c
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/scanner.h
        )

target_include_directories(scanner
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )