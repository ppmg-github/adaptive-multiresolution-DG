# CMake version
cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

# Declare project
project(AdaM-DG VERSION 1)

# Set output directories
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Set CXX standard as C++17
set(CMAKE_CXX_STANDARD 17)
# set cxx flag as gnu++17
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")

option(ADAM_DG_TEST "Build test" ON)
option(ADAM_DG_EXAMPLE "Build example" ON)

# Import OpenMP
find_package(OpenMP REQUIRED)

# Include thirdparty libraries
add_subdirectory(external)

# Create target and set properties
file(GLOB CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_library(adamdg ${CPP_FILES})

# Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(adamdg::adamdg ALIAS adamdg)

target_link_libraries(adamdg PUBLIC Eigen3::Eigen OpenMP::OpenMP_CXX)

# Set target properties
target_include_directories(adamdg
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

if(ADAM_DG_EXAMPLE)
    message(STATUS "Adding example")
    file(GLOB EXAMPLE_SOURCES example/*.cpp)

    foreach(sourcefile ${EXAMPLE_SOURCES})
        get_filename_component(name ${sourcefile} NAME_WE)
        message(STATUS "Adding example: ${name}")
        add_executable(${name} ${sourcefile})

        target_link_libraries(${name} PUBLIC adamdg)
    endforeach(sourcefile ${APP_SOURCES})
else()
    message(STATUS "Example is not included")
endif()

if(ADAM_DG_TEST)
    message(STATUS "Adding test suites")
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(test)
else()
    message(STATUS "Testing cases are not included")
endif()