# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(bggserver LANGUAGES CXX)

message(STATUS "############################################################")
message(STATUS "CMakeLists.txt for 'bggserver' in ${CMAKE_CURRENT_SOURCE_DIR}")



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "")
#set(BOOST_SUPERPROJECT_VERSION "1.89.0")

include(FetchContent)

###############################################################################
# Fetching Libraries  		                                                  #
###############################################################################

message(STATUS "CMake system prefix path: ${CMAKE_SYSTEM_PREFIX_PATH}")
message(STATUS "CMake prefix path: ${CMAKE_PREFIX_PATH}")


cmake_policy(SET CMP0167 NEW)
find_package(Boost 1.89.0 REQUIRED COMPONENTS 
  json
  fiber
)

if(Boost_FOUND)
  message(STATUS "Boost found: ${Boost_VERSION}")
endif()

find_package(GTest REQUIRED)
if(GTest_FOUND)
  message(STATUS "GoogleTest found: ${GTest_VERSION}")
endif()

###############################################################################
# Configure Build Process                                                     #
###############################################################################

if (MSVC)
  add_compile_options(
    /std:c++20                # C++20
    /FS                       # Allow parallel PDB access
    /MP                       # Multi-process build
    /Zi                       # Debug info, == -g
    /W4                       # Warning level 4, == -Wall -Wextra. 
                              # Note that: the max warning level is /Wall. It includes unnecessary warnings from system headers, STL, Windows SDK...
    /GS                       # Stack protector, == -fstack-protector-strong
    /EHc                     # Exception model (C++)
    /permissive-              # Strict standard conformance, == -Wpedantic
    /external:W0              # Suppress warnings from external headers
    /external:anglebrackets   # Treat headers included with <> as external
    /Zc:__cplusplus           # Standardize the __cplusplus macro value
    /Zc:inline                # Optimize inline function handling
    /Zc:preprocessor          # Use modern conforming preprocessor (MSVC 2019+)
    /d2FH4-                   # Workaround: Fix IntelliSense issues with /std:c++20 and STL (MSVC-specific)
    /sdl            # Format string + buffer checks. ~~-Wformat=2
    # /WX                     # Warnings as errors
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
    -std=c++20                  # Enable C++20 standard
    -Wall                       # Enable most common warnings
    -Wextra                     # Enable extra (more aggressive) warnings
    #-Wnoexcept                  # Enables warnings if a function marked noexcept might actually throw
    #-O2                         # Enables deeper analysis of noexcept
    -Wpedantic                  # Enforce strict ISO C++ compliance
    -Wconversion                # Warn on implicit type conversions (e.g., int to float)
    -Wsign-conversion           # Warn on implicit sign conversions (e.g., unsigned to signed)
    -Wshadow                    # Warn when a variable declaration shadows one from outer scope
    -Wnon-virtual-dtor          # Warn if a class with virtual functions has a non-virtual destructor
    -Wold-style-cast            # Warn on C-style casts (use static_cast, etc. instead)
    -Wnull-dereference          # Warn on potential null pointer dereference
    -Wdouble-promotion          # Warn when float is implicitly promoted to double
    -Wformat=2                  # Enable strict format string checking (e.g., printf/scanf)
    -Wimplicit-fallthrough      # Warn when a switch case falls through without annotation
    -fdiagnostics-color=always  # Force colored diagnostics output (useful in IDE/terminals)
    -fstack-protector-strong    # Add stack overflow protection (stronger than -fstack-protector)
    -fno-common                 # Prevents multiple global variable definitions with same name
    -g                          # Generate debug information
  )

endif()


###############################################################################
# Detect and print source files                                               #
# TODO: Use a more robust method to detect source files                       #
###############################################################################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  src/*.cpp
  src/*.h
)

###############################################################################
# Activate testing if BUILD_TESTING is ON                                     #
###############################################################################
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(tests)
endif()


###############################################################################
# Add executable and link libraries                                           #
###############################################################################

add_executable(bggserver 
  main.cpp
  ${SRC_FILES}
)

target_compile_features(bggserver PRIVATE cxx_std_20)
target_link_libraries(bggserver PRIVATE
  Boost::json
  Boost::fiber
)

target_include_directories(bggserver PRIVATE 
  ${CMAKE_SOURCE_DIR}/src
)

###############################################################################
# Print all targets
###############################################################################
function(print_all_targets_recursive dir)
  # Get and print all targets from the current directory
  get_property(local_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
  foreach(tgt IN LISTS local_targets)
    message(STATUS "+ ${tgt}")
  endforeach()

  # Find subdirectories then get and print all targets from them
  get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
  foreach(subdir IN LISTS subdirs)
    print_all_targets_recursive("${subdir}")
  endforeach()
endfunction()

# Call the recursive funtion
message(STATUS "================================================================")
message(STATUS "List of targets:")
print_all_targets_recursive(${CMAKE_SOURCE_DIR})


message(STATUS "================================================================")
message(STATUS "List of source files:")
get_target_property(ALL_SRC_FILES bggserver SOURCES)
foreach(src_file ${ALL_SRC_FILES})
  message(STATUS "+ ${src_file}")
endforeach()

message(STATUS "================================================================")
message(STATUS "List of include directories:")
get_target_property(INCLUDE_DIRS bggserver INCLUDE_DIRECTORIES)
foreach(include_dir ${INCLUDE_DIRS})
  message(STATUS "+ ${include_dir}")
endforeach()

message(STATUS "================================================================")
message(STATUS "List of linked libraries:")
get_target_property(LINK_LIBS bggserver LINK_LIBRARIES)
foreach(lib ${LINK_LIBS})
  message(STATUS "+ ${lib}")
endforeach()

###############################################################################
# Add necessary macros                                                        #
###############################################################################
target_compile_definitions(bggserver PRIVATE
  $<$<CONFIG:Debug>:_DEBUG> #Macro _DEBUG
  $<$<CONFIG:Release>:NDEBUG> #Macro NDEBUG for disbleing assert() 
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601> #Macro _WIN32_WINNT
)


###############################################################################
# Install App                                                                 #
###############################################################################
install(TARGETS bggserver
  RUNTIME DESTINATION bin
)
