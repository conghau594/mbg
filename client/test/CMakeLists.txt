# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(mbgclientTest LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
###############################################################################
# FetchContent for Google Test                                               #
###############################################################################
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG main
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

message(STATUS "googletest source directory: ${googletest_SOURCE_DIR}")
message(STATUS "googletest binary directory: ${googletest_BINARY_DIR}")
message(STATUS "googletest populated: ${googletest_POPULATED}")
message(STATUS "googletest Added: ${googletest_ADDED}")

###############################################################################
# Configure Build Process                                           #
###############################################################################
if (MSVC)
  add_compile_options(/FS /MP /Zi)
endif()


###############################################################################
# Add executable and link libraries                                           #
###############################################################################
add_executable(mbgclientTest main.cpp)
target_compile_features(mbgclientTest PRIVATE cxx_std_20)
target_link_libraries(mbgclientTest PRIVATE
  SFML::Graphics 
  SFML::Audio 
  SFML::Window
  SFML::System
  SFML::Main
  ImGui-SFML::ImGui-SFML 
)

###############################################################################
# Install App                                                                 #
###############################################################################
install(TARGETS mbgclient
  RUNTIME DESTINATION bin
)
install(DIRECTORY assets/
  DESTINATION share/${PROJECT_NAME}/assets
)
install(DIRECTORY fonts/
  DESTINATION share/${PROJECT_NAME}/fonts
)