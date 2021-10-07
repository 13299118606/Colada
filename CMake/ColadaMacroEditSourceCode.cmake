# 
# coladaMacroEditcpp
#
macro(coladaMacroEditcpp)
  set(options
    OLDFILE_DELETE
    OVERWRITE
    )
  set(oneValueArgs
    TARGET
    TARGET_FILE
    SRC_FILE
    DEST_FILE
    CXX_STANDARD
    )
  set(multiValueArgs
    )
  cmake_parse_arguments(COLADAEDITCPP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )

  # --------------------------------------------------------------------------
  # Sanity checks
  # --------------------------------------------------------------------------
  if(COLADAEDITCPP_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to coladaMacroEditcpp(): \"${COLADAEDITCPP_UNPARSED_ARGUMENTS}\"")
  endif()

  set(expected_defined_vars TARGET CXX_STANDARD)
  foreach(var ${expected_defined_vars})
    if(NOT DEFINED COLADAEDITCPP_${var})
      message(FATAL_ERROR "${var} is mandatory")
    endif()
  endforeach()

  set(expected_existing_vars TARGET_FILE SRC_FILE DEST_FILE)
  foreach(var ${expected_existing_vars})
    if(NOT EXISTS "${COLADAEDITCPP_${var}}")
      message(FATAL_ERROR "error: Variable ${var} set to ${COLADAEDITCPP_${var}} corresponds to an nonexistent file. ")
    endif()
  endforeach()

  #-----------------------------------------------------------------------------
  # Configure
  # --------------------------------------------------------------------------

  get_filename_component( 
    src_filename
    ${COLADAEDITCPP_SRC_FILE}
    NAME
    )

  get_filename_component( 
    dest_filename
    ${COLADAEDITCPP_DEST_FILE}
    NAME
    )

  get_filename_component( 
    target_dir
    ${COLADAEDITCPP_TARGET_FILE}
    DIRECTORY
    ABSOLUTE
    )

  # `\n` is needed to handle increase of empty lines when overwriting
  set(modify_start "\n# EDITED BY Colada - START: ${src_filename} ${dest_filename}")
  set(modify_end "# EDITED BY Colada - END: ${src_filename} ${dest_filename}")

  file(READ ${COLADAEDITCPP_TARGET_FILE} target_file_read)
  
  string(FIND ${target_file_read} 
    ${modify_start}
    is_file_edited)
  
  if(is_file_edited EQUAL -1 OR COLADAEDITCPP_OVERWRITE)

    if(COLADAEDITCPP_OLDFILE_DELETE)
      set(oldfile_cmd "--oldfile-delete")
    else()
      set(oldfile_cmd "--oldfile-keep")
    endif()

    if(WIN32)
      set(env_cmd "")
      set(editcpp_executable ${CMAKE_BINARY_DIR}/python-install/Scripts/editcpp)
    else()
      set(env_cmd "export LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/python-install/lib:$LD_LIBRARY_PATH && ")
      set(editcpp_executable ${CMAKE_BINARY_DIR}/python-install/bin/editcpp)
    endif()

    set(editcpp_cmd "
      ${env_cmd}
      ${editcpp_executable}
      --src-file=${COLADAEDITCPP_SRC_FILE}
      --dest-file=${COLADAEDITCPP_DEST_FILE}
      ${oldfile_cmd}
      -std=c++${COLADAEDITCPP_CXX_STANDARD}
      $<$<BOOL:$<TARGET_PROPERTY:${COLADAEDITCPP_TARGET},INCLUDE_DIRECTORIES>>:
        -I$<JOIN:$<TARGET_PROPERTY:${COLADAEDITCPP_TARGET},INCLUDE_DIRECTORIES>,$<SEMICOLON>-I>>")
  
    # in order to handle increase of empty lines when overwriting
    # double quotes should start at lines where `modify_start` and `modify_end` appear
    set(custom_cmd
"${modify_start}
get_target_property(target_deps ${COLADAEDITCPP_TARGET} LINK_LIBRARIES)
add_custom_command(
  TARGET ${COLADAEDITCPP_TARGET} PRE_BUILD
  COMMAND ${editcpp_cmd}
  COMMAND_EXPAND_LISTS 
  DEPENDS \"\${target_deps}\"
  WORKING_DIRECTORY ${target_dir}
  VERBATIM)
${modify_end}")
  
    if(is_file_edited GREATER -1 AND COLADAEDITCPP_OVERWRITE)
      string(REGEX REPLACE "${modify_start}.*${modify_end}"
              ${custom_cmd} target_file_modified ${target_file_read}
              )
      file(WRITE ${COLADAEDITCPP_TARGET_FILE} ${target_file_modified}) 
    else()
      file(APPEND ${COLADAEDITCPP_TARGET_FILE} ${custom_cmd})
    endif()
    
  endif()

endmacro()


# 
# coladaMacroEditpy
#
macro(coladaMacroEditpy)
  set(options
    OLDFILE_DELETE
    OVERWRITE
    )
  set(oneValueArgs
    TARGET
    TARGET_FILE
    SRC_FILE
    DEST_FILE
    )
  set(multiValueArgs
    )
  cmake_parse_arguments(COLADAEDITPY
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )

  # --------------------------------------------------------------------------
  # Sanity checks
  # --------------------------------------------------------------------------
  if(COLADAEDITPY_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to coladaMacroEditpy(): \"${COLADAEDITPY_UNPARSED_ARGUMENTS}\"")
  endif()

  set(expected_defined_vars TARGET)
  foreach(var ${expected_defined_vars})
    if(NOT DEFINED COLADAEDITPY_${var})
      message(FATAL_ERROR "${var} is mandatory")
    endif()
  endforeach()

  set(expected_existing_vars TARGET_FILE SRC_FILE DEST_FILE)
  foreach(var ${expected_existing_vars})
    if(NOT EXISTS "${COLADAEDITPY_${var}}")
      message(FATAL_ERROR "error: Variable ${var} set to ${COLADAEDITPY_${var}} corresponds to an nonexistent file. ")
    endif()
  endforeach()

  #-----------------------------------------------------------------------------
  # Configure
  # --------------------------------------------------------------------------

  get_filename_component( 
    src_filename
    ${COLADAEDITPY_SRC_FILE}
    NAME
    )

  get_filename_component( 
    dest_filename
    ${COLADAEDITPY_DEST_FILE}
    NAME
    )

  get_filename_component( 
    target_dir
    ${COLADAEDITPY_TARGET_FILE}
    DIRECTORY
    ABSOLUTE
    )

  # `\n` is needed to handle increase of empty lines when overwriting
  set(modify_start "\n# EDITED BY Colada - START: ${src_filename} ${dest_filename}")
  set(modify_end "# EDITED BY Colada - END: ${src_filename} ${dest_filename}")

  file(READ ${COLADAEDITPY_TARGET_FILE} target_file_read)
  
  string(FIND ${target_file_read} 
    ${modify_start}
    is_file_edited)
  
  if(is_file_edited EQUAL -1 OR COLADAEDITPY_OVERWRITE)

    if(COLADAEDITPY_OLDFILE_DELETE)
      set(oldfile_cmd "--oldfile-delete")
    else()
      set(oldfile_cmd "--oldfile-keep")
    endif()

    if(WIN32)
      set(env_cmd "")
      set(editpy_executable ${CMAKE_BINARY_DIR}/python-install/Scripts/editpy)
    else()
      set(env_cmd "export LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/python-install/lib:$LD_LIBRARY_PATH && ")
      set(editpy_executable ${CMAKE_BINARY_DIR}/python-install/bin/editpy)
    endif()

    set(editpy_cmd "
      ${env_cmd}
      ${editpy_executable}
      --src-file=${COLADAEDITPY_SRC_FILE}
      --dest-file=${COLADAEDITPY_DEST_FILE}
      ${oldfile_cmd}")
  
    # in order to handle increase of empty lines when overwriting
    # double quotes should start at lines where `modify_start` and `modify_end` appear
    set(custom_cmd
"${modify_start}
get_target_property(target_deps ${COLADAEDITPY_TARGET} LINK_LIBRARIES)
add_custom_command(
  TARGET ${COLADAEDITPY_TARGET} PRE_BUILD
  COMMAND ${editpy_cmd}
  COMMAND_EXPAND_LISTS 
  DEPENDS \"\${target_deps}\"
  WORKING_DIRECTORY ${target_dir}
  VERBATIM)
${modify_end}")
  
    if(is_file_edited GREATER -1 AND COLADAEDITPY_OVERWRITE)
      string(REGEX REPLACE "${modify_start}.*${modify_end}"
              ${custom_cmd} target_file_modified ${target_file_read}
              )
      file(WRITE ${COLADAEDITPY_TARGET_FILE} ${target_file_modified}) 
    else()
      file(APPEND ${COLADAEDITPY_TARGET_FILE} ${custom_cmd})
    endif()
    
  endif()

endmacro()