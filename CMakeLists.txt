cmake_minimum_required(VERSION 3.25)

project(App VERSION 0.0.1 LANGUAGES C CXX)

include(GNUInstallDirs)
include(FetchContent)
include(TestCXXAcceptsFlag)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)

option(BUILD_CMAKE_FORMAT "enable cmake format" ON)
option(BUILD_HIERARCHY_GRAPH "build graph" ON)
option(BUILD_WEB_DOC "build documentation" ON)
option(BUILD_TEST "enable build test executable" ON)
option(BUILD_CLANG_FORMAT "enable clang format" ON)
option(ENABLE_WARNINGS "enable warnings" ON)
option(ENABLE_WARNINGS_AS_ERRORS "enable warnings as errors" OFF)
option(ENABLE_CLANG_TIDY "enable clang tidy" OFF)
option(ENABLE_SANITIZE_ADDR "enable address sanitizer" OFF)
option(ENABLE_SANITIZE_UNDEF "enable undefined behavior sanitizer" OFF)

cmake_dependent_option(INSTALL_HIERARCHY_GRAPH "install docs" OFF "NOT BUILD_WEB_DOC" OFF)
cmake_dependent_option(INSTALL_HIERARCHY_GRAPH "install graph" OFF "NOT BUILD_HIERARCHY_GRAPH" OFF)

# Set CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set CMake variables
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Set custom variables
set(PREBUILD_DIR "${CMAKE_SOURCE_DIR}/prebuild")
set(PREBUILD_GAMES_DIR "${PREBUILD_DIR}/games")
set(PREBUILD_MONGOCXX_DIR "${PREBUILD_DIR}/mongo-cxx-driver")
set(PREBUILD_SFML_DIR "${PREBUILD_DIR}/SFML")
set(PRE_DEFINITION_PATH ${CMAKE_SOURCE_DIR}/app/pre-definition.hpp)

# Set output directory for each configuration
get_property(THIS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(THIS_MULTI_CONFIG)
    foreach(build_type IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER ${build_type} build_type_upper)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper}
            ${CMAKE_BINARY_DIR}/${build_type}/lib
            )
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper}
            ${CMAKE_BINARY_DIR}/${build_type}/lib
            )
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper}
            ${CMAKE_BINARY_DIR}/${build_type}/bin
            )
    endforeach()
endif()

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include(Config)
include(Tools)
include(Document)
include(Warnings)
include(Sanitizer)
include(FetchLib)
include(RegistModule)

build_clang_format_target("${CMAKE_SOURCE_DIR}/(build|prebuild|tools|.github|modules)/+")
build_cmake_format_target("${CMAKE_SOURCE_DIR}/(build|prebuild|tools|.github|modules)/+")
add_project_documentation()
add_sanitizer_flags()

if(THIS_OS_LINUX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endif(THIS_OS_LINUX)

list(APPEND CMAKE_PREFIX_PATH "${PREBUILD_GAMES_DIR}" "${PREBUILD_SFML_DIR}"
     "${PREBUILD_MONGOCXX_DIR}"
     )

fetchlib()
add_subdirectory(src)
add_subdirectory(app)
write_register_module_lib()
