# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(bggclient LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "")

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

message(STATUS "SFML updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_SFML}")
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

message(STATUS "ImGui updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_IMGUI}")
message(STATUS "ImGui source directory: ${ImGui_SOURCE_DIR}")
message(STATUS "ImGui binary directory: ${ImGui_BINARY_DIR}")
message(STATUS "ImGui populated: ${ImGui_POPULATED}")
message(STATUS "ImGui Added: ${ImGui_ADDED}")

###############################################################################
# FetchContent for ImGui-SFML                                                 #
###############################################################################
FetchContent_Declare(ImGui_SFML
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml
  GIT_TAG v3.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(ImGui_SFML)

message(STATUS "ImGui_SFML updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_IMGUI_SFML}")
message(STATUS "ImGui_SFML source directory: ${ImGui_SFML_SOURCE_DIR}")
message(STATUS "ImGui_SFML binary directory: ${ImGui_SFML_BINARY_DIR}")
message(STATUS "ImGui_SFML populated: ${ImGui_SFML_POPULATED}")
message(STATUS "ImGui_SFML Added: ${ImGui_SFML_ADDED}")

###############################################################################
# First: Fetch Boost::config                                                   #
###############################################################################

FetchContent_Declare(
  BoostConfig
  GIT_REPOSITORY https://github.com/boostorg/config.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostConfig)

message(STATUS "BoostConfig updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTCONFIG}")
message(STATUS "BoostConfig source directory: ${BoostConfig_SOURCE_DIR}")
message(STATUS "BoostConfig binary directory: ${BoostConfig_BINARY_DIR}")
message(STATUS "BoostConfig populated: ${BoostConfig_POPULATED}")
message(STATUS "BoostConfig Added: ${BoostConfig_ADDED}")

###############################################################################
# Then: Fetch Boost::assert                                                    #
###############################################################################
FetchContent_Declare(BoostAssert
  GIT_REPOSITORY https://github.com/boostorg/assert.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAssert)

message(STATUS "BoostAssert updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTASSERT}")
message(STATUS "BoostAssert source directory: ${BoostAssert_SOURCE_DIR}")
message(STATUS "BoostAssert binary directory: ${BoostAssert_BINARY_DIR}")
message(STATUS "BoostAssert populated: ${BoostAssert_POPULATED}")
message(STATUS "BoostAssert Added: ${BoostAssert_ADDED}")

# ###############################################################################
# # Fetch Boost::system                                                    #
# ###############################################################################
# FetchContent_Declare(BoostSystem
#   GIT_REPOSITORY https://github.com/boostorg/system.git
#   GIT_TAG        boost-1.88.0
#   GIT_SHALLOW ON
#   EXCLUDE_FROM_ALL
#   SYSTEM
# )
# FetchContent_MakeAvailable(BoostSystem)

# message(STATUS "BoostUuid updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTSYSTEM}")
# message(STATUS "BoostSystem source directory: ${BoostSystem_SOURCE_DIR}")
# message(STATUS "BoostSystem binary directory: ${BoostSystem_BINARY_DIR}")
# message(STATUS "BoostSystem populated: ${BoostSystem_POPULATED}")
# message(STATUS "BoostSystem Added: ${BoostSystem_ADDED}")

###############################################################################
# Fetch Boost::throw_exception                                                    #
###############################################################################
FetchContent_Declare(BoostThrowException
  GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostThrowException)

message(STATUS "BoostThrowException updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTTHROWEXCEPTION}")
message(STATUS "BoostThrowException source directory: ${BoostThrowException_SOURCE_DIR}")
message(STATUS "BoostThrowException binary directory: ${BoostThrowException_BINARY_DIR}")
message(STATUS "BoostThrowException populated: ${BoostThrowException_POPULATED}")
message(STATUS "BoostThrowException Added: ${BoostThrowException_ADDED}")

###############################################################################
# Fetch Boost::type_traits                                                    #
###############################################################################
FetchContent_Declare(BoostTypeTraits
  GIT_REPOSITORY https://github.com/boostorg/type_traits.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTypeTraits)

message(STATUS "BoostTypeTraits updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTTYPETRAITS}")
message(STATUS "BoostTypeTraits source directory: ${BoostTypeTraits_SOURCE_DIR}")
message(STATUS "BoostTypeTraits binary directory: ${BoostTypeTraits_BINARY_DIR}")
message(STATUS "BoostTypeTraits populated: ${BoostTypeTraits_POPULATED}")
message(STATUS "BoostTypeTraits Added: ${BoostTypeTraits_ADDED}")

###############################################################################
# Fetch Boost::static_assert                                                    #
###############################################################################
FetchContent_Declare(BoostStaticAssert
  GIT_REPOSITORY https://github.com/boostorg/static_assert.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostStaticAssert)

message(STATUS "BoostStaticAssert updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTSTATICASSERT}")
message(STATUS "BoostStaticAssert source directory: ${BoostStaticAssert_SOURCE_DIR}")
message(STATUS "BoostStaticAssert binary directory: ${BoostStaticAssert_BINARY_DIR}")
message(STATUS "BoostStaticAssert populated: ${BoostStaticAssert_POPULATED}")
message(STATUS "BoostStaticAssert Added: ${BoostStaticAssert_ADDED}")

###############################################################################
# Then: Fetch Boost.Uuid                                                      #
###############################################################################

FetchContent_Declare(BoostUuid
  GIT_REPOSITORY https://github.com/boostorg/uuid.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
  )

FetchContent_MakeAvailable(BoostUuid)
  
message(STATUS "BoostUuid updates disconnected: ${FETCHCONTENT_UPDATES_DISCONNECTED_BOOSTUUID}")
message(STATUS "BoostUuid source directory: ${BoostUuid_SOURCE_DIR}")
message(STATUS "BoostUuid binary directory: ${BoostUuid_BINARY_DIR}")
message(STATUS "BoostUuid populated: ${BoostUuid_POPULATED}")
message(STATUS "BoostUuid Added: ${BoostUuid_ADDED}")

###############################################################################
# Configure Build Process                                           #
###############################################################################
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

if (MSVC)
  add_compile_options(
    /std:c++20                # C++20
    /FS                       # Allow parallel PDB access
    /MP                       # Multi-process build
    /Zi                       # Debug info, == -g
    /W4                       # Warning level 4, == -Wall -Wextra. 
                              # Note that: max warning level is /Wall. It includes unnecessary warnings from system headers, STL, Windows SDK...
    /GS                       # Stack protector, == -fstack-protector-strong
    /EHsc                     # Exception model (C++)
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
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  src/*.cpp
  src/*.h
)

message(STATUS "List of source files:")
foreach(src_file ${SOURCES})
  message(STATUS "+ ${src_file}")
endforeach()

###############################################################################
# Add executable and link libraries                                           #
###############################################################################

add_executable(bggclient 
  main.cpp
  ${SOURCES}
)

target_compile_features(bggclient PRIVATE cxx_std_20)
target_link_libraries(bggclient PRIVATE
  SFML::Graphics 
  SFML::Audio 
  SFML::Window
  SFML::System
  SFML::Main
  ImGui-SFML::ImGui-SFML
  Boost::assert
  Boost::uuid
)

target_include_directories(bggclient PRIVATE 
  ${CMAKE_SOURCE_DIR}/src)


get_property(all_targets GLOBAL PROPERTY TARGETS)
foreach(tgt ${all_targets})
  message(STATUS "CMake target: ${tgt}")
endforeach()

get_target_property(INCLUDE_DIRS bggclient INCLUDE_DIRECTORIES)
message(STATUS "Include directories for bggclient: ${INCLUDE_DIRS}")

###############################################################################
# Add necessary macros                                                        #
###############################################################################
target_compile_definitions(bggclient PRIVATE
  $<$<CONFIG:Debug>:_DEBUG> #Macro _DEBUG
)

target_compile_definitions(bggclient PRIVATE
  $<$<CONFIG:Release>:NDEBUG> #Macro NDEBUG for disbleing assert() 
)


###############################################################################
# Install App                                                                 #
###############################################################################
install(TARGETS bggclient
  RUNTIME DESTINATION bin
)
install(DIRECTORY assets/
  DESTINATION share/${PROJECT_NAME}/assets
)
install(DIRECTORY fonts/
  DESTINATION share/${PROJECT_NAME}/fonts
)