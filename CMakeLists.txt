cmake_minimum_required(VERSION 4.3)
project(asmcore C)

add_library(asmcore STATIC
        encode/encode.c
        encode/encode.h
        scanner/scanner.c
        scanner/scanner.h
        symtab/symtab.h
        symtab/symtab.c
        Helpers/Dictionary/Dictionary.c
        Helpers/Dictionary/Dictionary.h)

target_link_libraries(asmcore PUBLIC isa)
target_include_directories(asmcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})