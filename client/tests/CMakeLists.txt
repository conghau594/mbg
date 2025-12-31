# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(bggclientTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "##################################################")
message(STATUS "CMakeLists.txt for 'bggclientTest' in ${CMAKE_CURRENT_SOURCE_DIR}")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "")

include(FetchContent)

###############################################################################
# Find Boost                                                                  #
###############################################################################
find_package(Boost 1.75 REQUIRED)
message(STATUS "+ Boost found: ${Boost_FOUND}")

###############################################################################
# Find or Fetch Google Test                                                   #
###############################################################################
find_package(GTest QUIET)

if (GTest_FOUND)
  message(STATUS "Found system GTest package, using it.")
else()
  message(STATUS "GTest not found, fetching from GitHub...")

  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.17.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  FetchContent_GetProperties(googletest POPULATED GOOGLETEST_POPULATED)
  message(STATUS "+ googletest populated: ${GOOGLETEST_POPULATED}")

  # Create aliases to match find_package target names
  if(NOT TARGET GTest::gtest_main)
    add_library(GTest::gtest_main ALIAS gtest_main)
  endif()
endif()



###############################################################################
# Add executable and link libraries                                           #
###############################################################################
add_subdirectory(model/chess)
