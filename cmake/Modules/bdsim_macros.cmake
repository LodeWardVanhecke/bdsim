#-----------------------------------------------------------------------
# macro BDSIM_COLLATE_APPLICATION_SOURCES(source_dest_var)
#
macro(BDSIM_COLLATE_APPLICATION_SOURCES source_dest_var)
  file(GLOB 
    ${source_dest_var} 
    ${CMAKE_CURRENT_BINARY_DIR}/bdsim.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BDS*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ggmad.cc
    )
endmacro()

#from https://cmake.org/pipermail/cmake/2009-March/027892.html

MACRO(COPY_FILE_IF_CHANGED in_file out_file target)
  #message(STATUS "in_file: ${in_file}")
  #message(STATUS "out_file: ${out_file}")
  # copy the file the first time before compilation if it doesn't exist
  IF(NOT EXISTS ${out_file})
    #message(STATUS "copying")
    # could use file(COPY here but that requires directory - avoid filename stripping
    configure_file(${in_file} ${out_file} COPYONLY)
  ELSEIF(${in_file} IS_NEWER_THAN ${out_file})
    #message(STATUS "copying")
    # this will only effictively be run after successfuly compilation even if called earlier
    ADD_CUSTOM_COMMAND (
      TARGET     ${target}
      POST_BUILD 
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy ${in_file} ${out_file}
      MAIN_DEPENDENCY ${in_file}
      #COMMENT    "Copying file: ${in_file} to: ${out_file}"
      )
  ENDIF()
ENDMACRO(COPY_FILE_IF_CHANGED)

# Copy all files and directories in in_dir to out_dir. 
# Subtrees remain intact.
MACRO(COPY_DIRECTORY_IF_CHANGED in_dir out_dir target)
  message(STATUS "Copying directory ${in_dir}")
  FILE(GLOB_RECURSE in_file_list ${in_dir}/*)
  FOREACH(in_file ${in_file_list})
    STRING(REGEX REPLACE ${in_dir} ${out_dir} out_file ${in_file})
    COPY_FILE_IF_CHANGED(${in_file} ${out_file} ${target})
  ENDFOREACH(in_file)
ENDMACRO(COPY_DIRECTORY_IF_CHANGED)

# Explicit version to copy examples files that are only in git
MACRO(COPY_EXAMPLES)
  message(STATUS "Copying example directory")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} ls-files --full-name ${CMAKE_SOURCE_DIR}/examples
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE git_file_list)
  separate_arguments(in_file_list UNIX_COMMAND "${git_file_list}")
  #message(STATUS "file list: ${in_file_list}")
  FOREACH(file ${in_file_list})
    set(in_file "${CMAKE_SOURCE_DIR}/${file}")
    set(out_file "${CMAKE_BINARY_DIR}/${file}")
    #message(STATUS "in_file: ${in_file}")
    #message(STATUS "out_file: ${out_file}")
    # Copy if changed, and link to bdsim target
    COPY_FILE_IF_CHANGED(${in_file} ${out_file} bdsimexec)
  ENDFOREACH(file)
ENDMACRO(COPY_EXAMPLES)

# basic copy routine for examples in case we have no git support
MACRO(COPY_EXAMPLES_NO_GIT)
  message(STATUS "Copying example directory")
  copy_directory_if_changed(${CMAKE_SOURCE_DIR}/examples ${CMAKE_BINARY_DIR}/examples POST_BUILD)
ENDMACRO(COPY_EXAMPLES_NO_GIT)


