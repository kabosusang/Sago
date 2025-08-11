function(copy_directory_to_target target_name src_dir)
    cmake_parse_arguments(
        ARG
        ""
        "DEST_DIR;TIMING"
        ""
        ${ARGN}
    )
    
    if(NOT ARG_DEST_DIR)
        set(ARG_DEST_DIR "$<TARGET_FILE_DIR:${target_name}>")
    endif()
    
    if(NOT ARG_TIMING)
        set(ARG_TIMING "POST_BUILD")
    endif()
    
    add_custom_command(
        TARGET ${target_name}
        ${ARG_TIMING}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${src_dir}"
            "${ARG_DEST_DIR}"
        COMMENT "Copying directory ${src_dir} to ${ARG_DEST_DIR}"
        VERBATIM
    )
endfunction()