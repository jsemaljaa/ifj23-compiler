add_library(ds "")

target_sources(ds
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/str.c
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/str.h
        ${CMAKE_CURRENT_LIST_DIR}/error.h
        )

target_include_directories(ds
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )