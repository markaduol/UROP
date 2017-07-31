cmake_minimum_required(VERSION 2.8)

# Create CXX_11 macro
macro(use_cxx11)
  if ("${CMAKE_VERSION}" VERSION_LESS "3.1")
    if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") OR ("${CMAKE_CXX_COMPILER}" MATCHES "GNU"))
      set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
  else ()
    set("${CMAKE_CXX_STANDARD}" 11)
  endif()
endmacro(use_cxx11)

# Include useful CMake functions
include(${CMAKE_SOURCE_DIR}/cmake/string_to_list.cmake)

# Find LLVM
include(${CMAKE_SOURCE_DIR}/cmake/find_llvm.cmake)
set(NEEDED_LLVM_VARS
  LLVM_PACKAGE_VERSION
  LLVM_VERSION_MAJOR
  LLVM_VERSION_PATCH
  LLVM_DEFINITIONS
  LLVM_ENABLE_ASSERTIONS
  LLVM_ENABLE_EH
  LLVM_ENABLE_RTTI
  LLVM_INCLUDE_DIRS
  LLVM_LIBRARY_DIRS
  LLVM_TOOLS_BINARY_DIR
  LLVM_ENABLE_VISIBILITY_INLINES_HIDDEN
  TARGET_TRIPLE
)

foreach (vname ${NEEDED_LLVM_VARS})
  message(STATUS "${vname}: \"${${vname}}\"")
  if (NOT (DEFINED "${vname}"))
    message(FATAL_ERROR "${vname} was not defined")
  endif()
endforeach()

# Find llvm-link
set(LLVM_LINK "${LLVM_TOOLS_BINARY_DIR}/llvm-link")
if (NOT EXISTS "${LLVM_LINK}")
  message(FATAL_ERROR "Failed to find llvm-link at \"${LLVM_LINK}\"")
endif()

# Find llvm-ar
set(LLVM_AR "${LLVM_TOOLS_BINARY_DIR}/llvm-ar")
if (NOT EXISTS "${LLVM_AR}")
  message(FATAL_ERROR "Failed to find llvm-ar at \"${LLVM_AR}\"")
endif()

# Find llvm-as
set(LLVM_AS "${LLVM_TOOLS_BINARY_DIR}/llvm-as")
if (NOT EXISTS "${LLVM_AS}")
  message(FATAL_ERROR "Failed to find llvm-as at \"${LLVM_AS}\"")
endif()

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(functionrename)  # Use your pass name here.
