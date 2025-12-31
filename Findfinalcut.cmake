include(FindPackageHandleStandardArgs)

find_path(FINALCUT_INCLUDE_DIR
  NAMES final/final.h
  HINTS /usr/local/include /usr/include
)

find_library(FINALCUT_LIBRARY
  NAMES final
  HINTS /usr/local/lib /usr/lib
)

find_package_handle_standard_args(finalcut
  REQUIRED_VARS FINALCUT_INCLUDE_DIR FINALCUT_LIBRARY
  VERSION_VAR FINALCUT_VERSION
)

if(FINALCUT_FOUND)
  add_library(finalcut::libfinal UNKNOWN IMPORTED)
  set_target_properties(finalcut::libfinal PROPERTIES
    IMPORTED_LOCATION "${FINALCUT_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${FINALCUT_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(FINALCUT_INCLUDE_DIR FINALCUT_LIBRARY)
