
find_program(MERCURIAL hg)

# try hg_archival.txt file
if (EXISTS ${CMAKE_SOURCE_DIR}/.hg_archival.txt)
  message("Found .hg_archival.txt")
  file(STRINGS "${CMAKE_SOURCE_DIR}/.hg_archival.txt" latest_tag_string
    REGEX "latesttag:.*")
  if (latest_tag_string)
    message("Found ${latest_tag_string}")
    string(REGEX REPLACE "latesttag:[ \t]*v([0-9.]+).*" "\\1"
      found_version_string ${latest_tag_string})
    message("Version from latest tag: ${found_version_string}")

    if (found_version_string MATCHES "([0-9]+)[0-9.]*")
      set(MAJOR_VERSION ${CMAKE_MATCH_1})
    endif()

    if (found_version_string MATCHES "[0-9]+\\.([0-9]+)[0-9.]*")
      set(MINOR_VERSION ${CMAKE_MATCH_1})
    endif()

    if (found_version_string MATCHES "[0-9]+\\.[0-9]+\\.([0-9]+)")
      set(PATCH_VERSION ${CMAKE_MATCH_1})
    endif()

    message("found_major_version: ${MAJOR_VERSION}")
    message("found_minor_version: ${MINOR_VERSION}")
    message("found_patch_version: ${PATCH_VERSION}")
  else(latest_tag_string)
    message(WARNING "Couldn't find latest tag in .hg_archival.txt.")
  endif()

elseif(MERCURIAL)



endif()

