message(STATUS "Synchronize assets...")
message(STATUS "ASSETS_DIR: ${ASSETS_DIR}")
message(STATUS "ASSETS_DEST: ${ASSETS_DEST}")

# Remove old assets
file(GLOB_RECURSE DEST_FILES "${ASSETS_DEST}/*")
foreach(file ${DEST_FILES})
    file(RELATIVE_PATH rel_path "${ASSETS_DEST}" "${file}")
    if(NOT EXISTS "${ASSETS_DIR}/${rel_path}")
        message(STATUS "Remove obsolete file: ${file}")
        file(REMOVE "${file}")
    endif()
endforeach()

# Update assets
file(GLOB_RECURSE SOURCE_FILES "${ASSETS_DIR}/*")
foreach(src ${SOURCE_FILES})
    file(RELATIVE_PATH rel_path "${ASSETS_DIR}" "${src}")

    get_filename_component(ext "${src}" EXT)
    if(ext STREQUAL ".frag" OR ext STREQUAL ".vert")
        message(STATUS "Skip shader source: ${rel_path}")
        continue()
    endif()
    
    set(dest "${ASSETS_DEST}/${rel_path}")

    get_filename_component(dest_dir "${dest}" DIRECTORY)
    file(MAKE_DIRECTORY "${dest_dir}")

    if(NOT EXISTS "${dest}")
        message(STATUS "Copy (new): ${rel_path}")
        file(COPY "${src}" DESTINATION "${dest_dir}")
    else()
        file(MD5 "${src}" src_md5)
        file(MD5 "${dest}" dest_md5)
        if(NOT "${src_md5}" STREQUAL "${dest_md5}")
            message(STATUS " Copy (modified): ${rel_path}")
            file(COPY "${src}" DESTINATION "${dest_dir}")
        else()
            message(STATUS "Identical, no copy: ${rel_path}")
        endif()
    endif()
endforeach()

message(STATUS "Synchronization terminated.")