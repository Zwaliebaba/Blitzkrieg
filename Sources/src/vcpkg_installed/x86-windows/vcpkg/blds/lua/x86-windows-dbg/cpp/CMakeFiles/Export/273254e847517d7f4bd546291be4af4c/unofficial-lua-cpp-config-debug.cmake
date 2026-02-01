#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lua-cpp" for configuration "Debug"
set_property(TARGET lua-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lua-cpp PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/lua-c++.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/lua-c++.dll"
  )

list(APPEND _cmake_import_check_targets lua-cpp )
list(APPEND _cmake_import_check_files_for_lua-cpp "${_IMPORT_PREFIX}/lib/lua-c++.lib" "${_IMPORT_PREFIX}/bin/lua-c++.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
