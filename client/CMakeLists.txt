# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(mbgclient LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

###############################################################################
# FetchContent for SFML                                                       #
###############################################################################
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.1
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SFML)
message(STATUS "SFML source directory: ${SFML_SOURCE_DIR}")
message(STATUS "SFML binary directory: ${SFML_BINARY_DIR}")
message(STATUS "SFML populated: ${SFML_POPULATED}")
message(STATUS "SFML Added: ${SFML_ADDED}")

###############################################################################
# FetchContent for ImGui                                                      #
###############################################################################
FetchContent_Declare(ImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.91.1
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)

message(STATUS "ImGui source directory: ${ImGui_SOURCE_DIR}")
message(STATUS "ImGui binary directory: ${ImGui_BINARY_DIR}")
message(STATUS "ImGui populated: ${ImGui_POPULATED}")
message(STATUS "ImGui Added: ${ImGui_ADDED}")

###############################################################################
# FetchContent for ImGui-SFML                                                 #
###############################################################################
FetchContent_Declare(ImGui-SFML
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml
  GIT_TAG v3.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(ImGui-SFML)

message(STATUS "ImGui-SFML source directory: ${ImGui-SFML_SOURCE_DIR}")
message(STATUS "ImGui-SFML binary directory: ${ImGui-SFML_BINARY_DIR}")
message(STATUS "ImGui-SFML populated: ${ImGui-SFML_POPULATED}")
message(STATUS "ImGui-SFML Added: ${ImGui-SFML_ADDED}")


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
add_executable(mbgclient main.cpp)
target_compile_features(mbgclient PRIVATE cxx_std_20)
target_link_libraries(mbgclient PRIVATE
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