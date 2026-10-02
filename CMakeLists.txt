
cmake_minimum_required(VERSION 3.22)

project(CPvP_AutoTotemLEVI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(LL_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

set(PRELOADER_ANDROID_ROOT
    "${LL_REPO_ROOT}/app/src/main/cpp/preloader"
    CACHE PATH "Path to LeviLaunchroid preloader-android"
)

if(NOT EXISTS "${PRELOADER_ANDROID_ROOT}/include/pl/Config.hpp")
    message(FATAL_ERROR
        "Cannot find LeviLaunchroid preloader-android SDK. "
        "Check PRELOADER_ANDROID_ROOT."
    )
endif()

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_Declare(
    boost_pfr
    GIT_REPOSITORY https://github.com/boostorg/pfr.git
    GIT_TAG boost-1.88.0
)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
)

set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(
    nlohmann_json
    boost_pfr
    magic_enum
    fmt
)

function(configure_mod target)
    target_include_directories(${target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${PRELOADER_ANDROID_ROOT}/include"
    )

    target_link_libraries(${target} PRIVATE
        nlohmann_json::nlohmann_json
        Boost::pfr
        magic_enum::magic_enum
        fmt::fmt
    )

    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic
    )
endfunction()

if(NOT ANDROID)
    add_executable(config_gen
        src/GenerateConfig.cpp
    )

    configure_mod(config_gen)

    target_compile_definitions(config_gen PRIVATE
        PL_CONFIG_NO_RUNTIME
    )
endif()

if(ANDROID)
    if(NOT EXISTS "${PRELOADER_ANDROID_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR "Preloader Android CMakeLists.txt not found.")
    endif()

    add_subdirectory("${PRELOADER_ANDROID_ROOT}" preloader_android)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/out/${ANDROID_ABI}"
    )

    add_library(cpvp_autototem SHARED
        src/FullCppMod.cpp
    )

    set_target_properties(cpvp_autototem PROPERTIES
        OUTPUT_NAME "CPvPAutoTotem"
        CXX_VISIBILITY_PRESET hidden
    )

    configure_mod(cpvp_autototem)

    if(TARGET preloader)
        target_link_libraries(cpvp_autototem PRIVATE preloader)
    else()
        message(FATAL_ERROR "Preloader target not found.")
    endif()
endif()
