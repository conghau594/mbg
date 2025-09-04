# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(bggclientTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "##################################################")
message(STATUS "CMakeLists.txt for 'bggclientTest' in ${CMAKE_CURRENT_SOURCE_DIR}")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "")
set(BOOST_SUPERPROJECT_VERSION "1.88.0")

include(FetchContent)

###############################################################################
# FetchContent for Google Test                                               #
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

# # Install GTest (create GTestConfig.cmake)
# set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/_install" CACHE PATH "" FORCE)
# execute_process(
#   COMMAND "${CMAKE_COMMAND}" --build "${googletest_BINARY_DIR}" --target install
# )
# list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/_install" CACHE PATH "" FORCE)

# message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

endif()



###############################################################################
# Add executable and link libraries                                           #
###############################################################################
add_subdirectory(model/chess)
