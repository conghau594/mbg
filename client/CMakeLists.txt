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

###############################################################################
# FetchContent for spdlog                                                      #
###############################################################################
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.15.3
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(spdlog)

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

###############################################################################
# Fetch Boost::type_index                                                       #
###############################################################################
FetchContent_Declare(BoostTypeIndex
  GIT_REPOSITORY https://github.com/boostorg/type_index.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTypeIndex)

###############################################################################
# Fetch Boost::pool                                                       #
###############################################################################
FetchContent_Declare(BoostPool
  GIT_REPOSITORY https://github.com/boostorg/pool.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostPool)

###############################################################################
# Fetch Boost::date_time                                                       #
###############################################################################
FetchContent_Declare(BoostDateTime
  GIT_REPOSITORY https://github.com/boostorg/date_time.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostDateTime)

###############################################################################
# Fetch Boost::winapi                                                       #
###############################################################################
FetchContent_Declare(BoostWinApi
  GIT_REPOSITORY https://github.com/boostorg/winapi.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostWinApi)

###############################################################################
# Fetch Boost::variant2                                                       #
###############################################################################
FetchContent_Declare(BoostVariant2
  GIT_REPOSITORY https://github.com/boostorg/variant2.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostVariant2)

###############################################################################
# Fetch Boost::integer                                                       #
###############################################################################
FetchContent_Declare(BoostInteger
  GIT_REPOSITORY https://github.com/boostorg/integer.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostInteger)

###############################################################################
# Fetch Boost::algorithm                                                       #
###############################################################################
FetchContent_Declare(BoostAlgorithm
  GIT_REPOSITORY https://github.com/boostorg/algorithm.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAlgorithm)

###############################################################################
# Fetch Boost::predef                                                       #
###############################################################################
FetchContent_Declare(BoostPredef
  GIT_REPOSITORY https://github.com/boostorg/predef.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostPredef)

###############################################################################
# Fetch Boost::io                                                       #
###############################################################################
FetchContent_Declare(BoostIo
  GIT_REPOSITORY https://github.com/boostorg/io.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostIo)

###############################################################################
# Fetch Boost::array                                                       #
###############################################################################
FetchContent_Declare(BoostArray
  GIT_REPOSITORY https://github.com/boostorg/array.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostArray)

###############################################################################
# Fetch Boost::lexical_cast                                                       #
###############################################################################
FetchContent_Declare(BoostLexicalCast
  GIT_REPOSITORY https://github.com/boostorg/lexical_cast.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostLexicalCast)

###############################################################################
# Fetch Boost::concept_check                                                       #
###############################################################################
FetchContent_Declare(BoostConceptCheck
  GIT_REPOSITORY https://github.com/boostorg/concept_check.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostConceptCheck)

###############################################################################
# Fetch Boost::system                                                    #
###############################################################################
FetchContent_Declare(BoostSystem
  GIT_REPOSITORY https://github.com/boostorg/system.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostSystem)

###############################################################################
# Fetch Boost::numeric_conversion                                                    #
###############################################################################
FetchContent_Declare(BoostNumericConversion
  GIT_REPOSITORY https://github.com/boostorg/numeric_conversion.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostNumericConversion)

###############################################################################
# Fetch Boost::tuple                                                    #
###############################################################################
FetchContent_Declare(BoostTuple
  GIT_REPOSITORY https://github.com/boostorg/tuple.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTuple)

###############################################################################
# Fetch Boost::function                                                    #
###############################################################################
FetchContent_Declare(BoostFunction
  GIT_REPOSITORY https://github.com/boostorg/function.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFunction)

###############################################################################
# Fetch Boost::range                                                    #
###############################################################################
FetchContent_Declare(BoostRange
  GIT_REPOSITORY https://github.com/boostorg/range.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostRange)

###############################################################################
# Fetch Boost::iterator                                                    #
###############################################################################
FetchContent_Declare(BoostIterator
  GIT_REPOSITORY https://github.com/boostorg/iterator.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostIterator)

###############################################################################
# Fetch Boost::tokenizer                                                    #
###############################################################################
FetchContent_Declare(BoostTokenizer
  GIT_REPOSITORY https://github.com/boostorg/tokenizer.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTokenizer)

###############################################################################
# Fetch Boost::mpl                                                    #
###############################################################################
FetchContent_Declare(BoostMpl
  GIT_REPOSITORY https://github.com/boostorg/mpl.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostMpl)

###############################################################################
# Fetch Boost::detail                                                    #
###############################################################################
FetchContent_Declare(BoostDetail
  GIT_REPOSITORY https://github.com/boostorg/detail.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostDetail)

###############################################################################
# Fetch Boost::conversion                                                    #
###############################################################################
FetchContent_Declare(BoostConversion
  GIT_REPOSITORY https://github.com/boostorg/conversion.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostConversion)

###############################################################################
# Fetch Boost::utility                                                    #
###############################################################################
FetchContent_Declare(BoostUtility
  GIT_REPOSITORY https://github.com/boostorg/utility.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostUtility)

###############################################################################
# Fetch Boost::regex                                                    #
###############################################################################
FetchContent_Declare(BoostRegex
  GIT_REPOSITORY https://github.com/boostorg/regex.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostRegex)

###############################################################################
# Fetch Boost::fusion                                                    #
###############################################################################
FetchContent_Declare(BoostFusion
  GIT_REPOSITORY https://github.com/boostorg/fusion.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFusion)

###############################################################################
# Fetch Boost::unordered                                                    #
###############################################################################
FetchContent_Declare(BoostUnordered
  GIT_REPOSITORY https://github.com/boostorg/unordered.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostUnordered)

###############################################################################
# Fetch Boost::function_types                                                    #
###############################################################################
FetchContent_Declare(BoostFunctionTypes
  GIT_REPOSITORY https://github.com/boostorg/function_types.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFunctionTypes)

###############################################################################
# Fetch Boost::exception                                                    #
###############################################################################
FetchContent_Declare(BoostException
  GIT_REPOSITORY https://github.com/boostorg/exception.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostException)

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

###############################################################################
# Then: Fetch Boost::uuid                                                      #
###############################################################################
FetchContent_Declare(BoostUuid
  GIT_REPOSITORY https://github.com/boostorg/uuid.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostUuid)
  
###############################################################################
# Fetch Boost::bind                                                      #
###############################################################################
FetchContent_Declare(BoostBind
  GIT_REPOSITORY https://github.com/boostorg/bind.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostBind)

###############################################################################
# Fetch Boost::logic                                                      #
###############################################################################
FetchContent_Declare(BoostLogic
  GIT_REPOSITORY https://github.com/boostorg/logic.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostLogic)
 
###############################################################################
# Fetch Boost::preprocessor                                                      #
###############################################################################
FetchContent_Declare(BoostPreprocessor
  GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostPreprocessor)
 
###############################################################################
# Fetch Boost::optional                                                      #
###############################################################################
FetchContent_Declare(BoostOptional
  GIT_REPOSITORY https://github.com/boostorg/optional.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostOptional)
 
###############################################################################
# Fetch Boost::beast                                                      #
###############################################################################
FetchContent_Declare(BoostBeast
  GIT_REPOSITORY https://github.com/boostorg/beast.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostBeast)

###############################################################################
# Fetch Boost::context                                                      #
###############################################################################
FetchContent_Declare(BoostContext
  GIT_REPOSITORY https://github.com/boostorg/context.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostContext)
 
###############################################################################
# Fetch Boost::asio                                                      #
###############################################################################
FetchContent_Declare(BoostAsio
  GIT_REPOSITORY https://github.com/boostorg/asio.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAsio)

###############################################################################
# Fetch Boost::core                                                            #
###############################################################################
FetchContent_Declare(BoostCore
  GIT_REPOSITORY https://github.com/boostorg/core.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostCore)
  
###############################################################################
# Fetch Boost::move                                                      #
###############################################################################
FetchContent_Declare(BoostMove
  GIT_REPOSITORY https://github.com/boostorg/move.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostMove)

###############################################################################
# Fetch Boost::intrusive                                                      #
###############################################################################
FetchContent_Declare(BoostIntrusive
  GIT_REPOSITORY https://github.com/boostorg/intrusive.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostIntrusive)

###############################################################################
# Fetch Boost::endian                                                      #
###############################################################################
FetchContent_Declare(BoostEndian
  GIT_REPOSITORY https://github.com/boostorg/endian.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostEndian)

###############################################################################
# Fetch Boost::mp11                                                      #
###############################################################################
FetchContent_Declare(BoostMp11
  GIT_REPOSITORY https://github.com/boostorg/mp11.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostMp11)

###############################################################################
# Fetch Boost::smart_ptr                                                      #
###############################################################################
FetchContent_Declare(BoostSmartPtr
  GIT_REPOSITORY https://github.com/boostorg/smart_ptr.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostSmartPtr)

###############################################################################
# Fetch Boost::describe                                                      #
###############################################################################
FetchContent_Declare(BoostDescribe
  GIT_REPOSITORY https://github.com/boostorg/describe.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostDescribe)

###############################################################################
# Then: Fetch Boost::container_hash                                                      #
###############################################################################
FetchContent_Declare(BoostContainerHash
  GIT_REPOSITORY https://github.com/boostorg/container_hash.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostContainerHash)

###############################################################################
# Fetch Boost::container                                                      #
###############################################################################
FetchContent_Declare(BoostContainer
  GIT_REPOSITORY https://github.com/boostorg/container.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostContainer)

###############################################################################
# Then: Fetch Boost::align                                                      #
###############################################################################
FetchContent_Declare(BoostAlign
  GIT_REPOSITORY https://github.com/boostorg/align.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAlign)

###############################################################################
# Then: Fetch Boost::json                                                      #
###############################################################################
FetchContent_Declare(BoostJson
  GIT_REPOSITORY https://github.com/boostorg/json.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostJson)

###############################################################################
# Fetch Boost::headers                                                      #
###############################################################################
FetchContent_Declare(BoostHeaders
  GIT_REPOSITORY https://github.com/boostorg/headers.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostHeaders)

###############################################################################
# Fetch Boost::static_string                                                      #
###############################################################################
FetchContent_Declare(BoostStaticString
  GIT_REPOSITORY https://github.com/boostorg/static_string.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostStaticString)

###############################################################################
# Fetch Boost::functional                                                      #
###############################################################################
FetchContent_Declare(BoostFunctional
  GIT_REPOSITORY https://github.com/boostorg/functional.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFunctional)

###############################################################################
# Fetch Boost::typeof                                                      #
###############################################################################
FetchContent_Declare(BoostTypeof
  GIT_REPOSITORY https://github.com/boostorg/typeof.git
  GIT_TAG        boost-1.88.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTypeof)

###############################################################################
# Fetch OpenSSL::SSL OpenSSL::Crypto                                                  #
###############################################################################
FetchContent_Declare(OpenSSL
  GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
  GIT_TAG        master
  GIT_SHALLOW    ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(OpenSSL)



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
                              # Note that: the max warning level is /Wall. It includes unnecessary warnings from system headers, STL, Windows SDK...
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
  Boost::beast
  Boost::json
  ssl 
  crypto
  spdlog
)

target_include_directories(bggclient PRIVATE 
  ${CMAKE_SOURCE_DIR}/src
)


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
  $<$<CONFIG:Release>:NDEBUG> #Macro NDEBUG for disbleing assert() 
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601>
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