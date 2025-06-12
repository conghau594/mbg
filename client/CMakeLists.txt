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