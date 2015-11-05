find_package(Git REQUIRED)

add_custom_target(buildinfo
  COMMAND ${CMAKE_COMMAND}
          -DGIT_EXECUTABLE:FILEPATH="${GIT_EXECUTABLE}"
          -DSOURCE_DIR:PATH="${MAPTK_SOURCE_DIR}"
          -DBINARY_DIR:PATH="${MAPTK_BINARY_DIR}"
          -DCOMPILER_NAME:STRING="${CMAKE_CXX_COMPILER_ID}"
          -DCOMPILER_VERSION:STRING="${CMAKE_CXX_COMPILER_VERSION}"
          -P "${MAPTK_SOURCE_DIR}/CMake/tools/maptk-create-buildinfo.cmake"
  WORKING_DIRECTORY "${MAPTK_SOURCE_DIR}"
)

function(maptk_use_appdata)
  foreach(FILE ${ARGN})
    message(STATUS "File: '${FILE}'")
    if (FILE STREQUAL "BUILDINFO")
      add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/BUILDINFO"
               "${CMAKE_CURRENT_BINARY_DIR}/BUILDINFO.phony"
        DEPENDS buildinfo
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MAPTK_BINARY_DIR}/BUILDINFO"
                "${CMAKE_CURRENT_BINARY_DIR}/BUILDINFO"
      )
    else()
      add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FILE}"
        DEPENDS "${MAPTK_SOURCE_DIR}/${FILE}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MAPTK_SOURCE_DIR}/${FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${FILE}"
      )
    endif()
  endforeach()
endfunction()
