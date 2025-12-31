# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(<test-suit-name> LANGUAGES CXX)

message(STATUS "########################################")
message(STATUS "CMakeLists.txt for '<test-suit-name>' in ${CMAKE_CURRENT_SOURCE_DIR}")

###############################################################################
# Detect and print source files                                               #
# TODO: Use a more robust method to detect source files                       #
###############################################################################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  *.cpp
  *.h
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.h"
)

message(STATUS "---------------------")
message(STATUS "List of source files:")
message(STATUS "---------------------")
foreach(src_file ${SRC_FILES})
  message(STATUS "+ ${src_file}")
endforeach()

###############################################################################
# Add executable and link libraries                                           #
###############################################################################
add_executable(<test-suit-name> 
  ${SRC_FILES}
)

target_compile_features(<test-suit-name> PRIVATE cxx_std_20)
target_link_libraries(<test-suit-name> PRIVATE
  gtest_main
)  

target_include_directories(<test-suit-name> PRIVATE
  ${CMAKE_SOURCE_DIR}/src
)

###############################################################################
# Enable Google Test discovery                                                #
# This will automatically discover tests in the <test-suit-name> executable      #
# and run them when using `ctest` or `make test`                              #
###############################################################################
include(GoogleTest)
gtest_discover_tests(<test-suit-name>  
  EXTRA_ARGS --gtest_color=yes
)

###############################################################################
# Flags for sanitizer                                                         #
###############################################################################
if (MSVC)
    # MSVC uses /fsanitize=address since Visual Studio 2019 16.9+
    #target_compile_options(<test-suit-name> PRIVATE /fsanitize=address /Zi /Od)
else()
    # GCC / Clang
    target_compile_options(<test-suit-name> PRIVATE -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -g)
    target_link_options(<test-suit-name> PRIVATE -fsanitize=address -fsanitize=leak)
endif()

###############################################################################
# Add necessary macros                                                        #
###############################################################################
target_compile_definitions(<test-suit-name> PRIVATE
  $<$<CONFIG:Debug>:_DEBUG> #Macro _DEBUG
  $<$<CONFIG:Release>:NDEBUG> #Macro NDEBUG for disabling assert() 
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601> #Macro _WIN32_WINNT
)

###############################################################################
# Install App                                                                 #
###############################################################################
# install(TARGETS <test-suit-name>
#   RUNTIME DESTINATION bin
# )
