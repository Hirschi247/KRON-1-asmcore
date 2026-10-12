cmake_minimum_required(VERSION 3.16)
project(KRON-1-asmcore C)

add_library(KRON-1-asmcore STATIC
        encode/encode.c
        encode/encode.h
        scanner/scanner.c
        scanner/scanner.h
        symtab/symtab.h
        symtab/symtab.c
        Helpers/Dictionary/Dictionary.c
        Helpers/Dictionary/Dictionary.h)

target_link_libraries(KRON-1-asmcore PUBLIC KRON-1-isa)
target_include_directories(KRON-1-asmcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})