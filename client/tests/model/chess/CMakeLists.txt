# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(chessRuleTest LANGUAGES CXX)

message(STATUS "########################################")
message(STATUS "CMakeLists.txt for 'chessRuleTest' in ${CMAKE_CURRENT_SOURCE_DIR}")

###############################################################################
# Detect and print source files                                               #
# TODO: Use a more robust method to detect source files                       #
###############################################################################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  *.cpp
  *.h
  "${CMAKE_SOURCE_DIR}/src/model/*.cpp"
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
add_executable(chessRuleTest 
  ${SRC_FILES}
)

target_compile_features(chessRuleTest PRIVATE cxx_std_20)
target_link_libraries(chessRuleTest PRIVATE
  gtest_main
  boost_assert
  spdlog
)  

target_include_directories(chessRuleTest PRIVATE 
  ${CMAKE_SOURCE_DIR}/src
)

###############################################################################
# Enable Google Test discovery                                                #
# This will automatically discover tests in the chessRuleTest executable      #
# and run them when using `ctest` or `make test`                              #
###############################################################################
include(GoogleTest)
gtest_discover_tests(chessRuleTest  
  EXTRA_ARGS --gtest_color=yes
)

###############################################################################
# Flags for sanitizer                                                         #
###############################################################################
if (MSVC)
    # MSVC uses /fsanitize=address since Visual Studio 2019 16.9+
    #target_compile_options(chessRuleTest PRIVATE /fsanitize=address /Zi /Od)
else()
    # GCC / Clang
    target_compile_options(chessRuleTest PRIVATE -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -g)
    target_link_options(chessRuleTest PRIVATE -fsanitize=address -fsanitize=leak)
endif()

###############################################################################
# Add necessary macros                                                        #
###############################################################################
target_compile_definitions(chessRuleTest PRIVATE
  $<$<CONFIG:Debug>:_DEBUG> #Macro _DEBUG
  $<$<CONFIG:Release>:NDEBUG> #Macro NDEBUG for disabling assert() 
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601> #Macro _WIN32_WINNT
)

###############################################################################
# Install App                                                                 #
###############################################################################
install(TARGETS chessRuleTest
  RUNTIME DESTINATION bin
)
