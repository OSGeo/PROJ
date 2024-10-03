# Derived from https://gitlab.com/jhamberg/cmake-examples/-/blob/master/cmake/FileEmbed.cmake
# MIT licensed
# Copyright (c) 2022 Jonathan Hamberg

function(FileEmbedSetup)

    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/file_embed)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}file_embed)
    endif ()

    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/file_embed/file_embed_empty.c)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/file_embed/file_embed_empty.c "")
    endif ()

    add_library(file_embed ${CMAKE_CURRENT_BINARY_DIR}/file_embed/file_embed_empty.c)
    target_include_directories(file_embed PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/file_embed)

endfunction()

function(FileEmbedAdd file)
    FileEmbedGenerate(${file} var)
    target_sources(file_embed PUBLIC ${var})

    add_custom_command(
            OUTPUT ${var}
            COMMAND ${CMAKE_COMMAND}
            -DRUN_FILE_EMBED_GENERATE=1
            "-DFILE_EMBED_GENERATE_PATH=${file}"
            -P ${PROJECT_SOURCE_DIR}/cmake/FileEmbed.cmake
            MAIN_DEPENDENCY ${file}
    )
endfunction()

function(FileEmbedGenerate file generated_c)

    get_filename_component(base_filename ${file} NAME)
    set(output_filename "${base_filename}.c")
    string(MAKE_C_IDENTIFIER ${base_filename} c_name)
    file(READ ${file} content HEX)

    string(LENGTH "${content}" size)
    math(EXPR size_mult_16 "${size} - (${size} % 16)")
    string(SUBSTRING "${content}" 0 ${size_mult_16} content_mult_16)

    string(REGEX REPLACE
           "([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])\
([A-Fa-f0-9][A-Fa-f0-9])"
           "0x\\1,0x\\2,0x\\3,0x\\4,0x\\5,0x\\6,0x\\7,0x\\8,\n" SEPARATED_HEX "${content_mult_16}")

    set(SEPARATED_HEX_REMAINDER "")
    if (NOT ${size_mult_16} EQUAL ${size})
        string(SUBSTRING "${content}" ${size_mult_16} 16 content_remainder)
        string(REGEX REPLACE "([A-Fa-f0-9][A-Fa-f0-9])" "0x\\1," SEPARATED_HEX_REMAINDER "${content_remainder}")
    endif()

    set(output_c "${SEPARATED_HEX}${SEPARATED_HEX_REMAINDER}")

    set(output_c "
#include \"${c_name}.h\"
const uint8_t ${c_name}_data[] = {
    ${output_c}
}\;
const unsigned ${c_name}_size = sizeof(${c_name}_data)\;
")

    set(output_h "
#ifndef ${c_name}_H
#define ${c_name}_H
#include <stdint.h>
extern const uint8_t ${c_name}_data[]\;
extern const unsigned ${c_name}_size\;
#endif // ${c_name}_H
    ")


    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/file_embed)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}file_embed)
    endif ()


    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/file_embed/${c_name}.c
            ${output_c})

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/file_embed/${c_name}.h
            ${output_h})

    set(${generated_c} ${CMAKE_CURRENT_BINARY_DIR}/file_embed/${c_name}.c PARENT_SCOPE)

endfunction()

if (RUN_FILE_EMBED_GENERATE)
    FileEmbedGenerate(${FILE_EMBED_GENERATE_PATH} var)
endif ()
