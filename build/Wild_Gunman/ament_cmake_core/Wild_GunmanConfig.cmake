# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_Wild_Gunman_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED Wild_Gunman_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(Wild_Gunman_FOUND FALSE)
  elseif(NOT Wild_Gunman_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(Wild_Gunman_FOUND FALSE)
  endif()
  return()
endif()
set(_Wild_Gunman_CONFIG_INCLUDED TRUE)

# output package information
if(NOT Wild_Gunman_FIND_QUIETLY)
  message(STATUS "Found Wild_Gunman: 0.0.0 (${Wild_Gunman_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'Wild_Gunman' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${Wild_Gunman_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(Wild_Gunman_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${Wild_Gunman_DIR}/${_extra}")
endforeach()
