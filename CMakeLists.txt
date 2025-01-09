cmake_minimum_required(VERSION 3.25)

project(App VERSION 0.0.1 LANGUAGES C CXX)

include(GNUInstallDirs)
include(FetchContent)
include(TestCXXAcceptsFlag)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)

option(ENABLE_TESTING "Enable a Unit Testing build." ON)
option(ENABLE_COVERAGE "Enable a Code Coverage build." OFF)

option(ENABLE_DEPENDENCIES_GRAPH "build graph" OFF)
option(ENABLE_HTML_DOCUMENT "build documentation" OFF)

option(ENABLE_CMAKE_FORMAT "enable cmake format" OFF)
option(ENABLE_CLANG_FORMAT "Enable to add clang-format." OFF)

option(ENABLE_WARNINGS "enable warnings" ON)
option(ENABLE_WARNINGS_AS_ERRORS "enable warnings as errors" OFF)

option(ENABLE_CLANG_TIDY "Enable to add clang tidy." OFF)

option(ENABLE_SANITIZE_ADDR "Enable address sanitize." OFF)
option(ENABLE_SANITIZE_UNDEF "Enable undefined sanitize." OFF)
option(ENABLE_SANITIZE_LEAK "Enable leak sanitize (Gcc/Clang only)." OFF)
option(ENABLE_SANITIZE_THREAD "Enable thread sanitize (Gcc/Clang only)." OFF)

cmake_dependent_option(INSTALL_DEPENDENCIES_GRAPH "install docs" ON "NOT ENABLE_DEPENDENCIES_GRAPH" OFF)
cmake_dependent_option(INSTALL_HTML_DOCUMENT "install graph" ON "NOT ENABLE_HTML_DOCUMENT" OFF)

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
include(Warnings)
include(RegistModule)

if(ENABLE_CLANG_FORMAT OR ENABLE_CMAKE_FORMAT)
    include(Tools)

    if(ENABLE_CLANG_FORMAT)
        build_clang_format_target("${CMAKE_SOURCE_DIR}/(build|install|tools|docs|node|.github)/+")
    endif()

    if(ENABLE_CMAKE_FORMAT)
        build_cmake_format_target("${CMAKE_SOURCE_DIR}/(build|install|tools|docs|node|.github)/+")
    endif()
endif()

if(ENABLE_HTML_DOCUMENT OR ENABLE_DEPENDENCIES_GRAPH)
    include(Document)

    if(ENABLE_HTML_DOCUMENT)
        build_html_document(${CMAKE_SOURCE_DIR}/docs)
    endif()

    if(ENABLE_DEPENDENCIES_GRAPH)
        build_dependencies_graph(${CMAKE_SOURCE_DIR}/docs/graph)
    endif()
endif()

if(ENABLE_SANITIZE_ADDR OR ENABLE_SANITIZE_UNDEF)
    include(Sanitizer)
    add_sanitizer_flags(${ENABLE_SANITIZE_ADDR} ${ENABLE_SANITIZE_UNDEF} ${ENABLE_SANITIZE_LEAK} ${ENABLE_SANITIZE_THREAD})
endif()

if(ENABLE_TESTING AND ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

if(THIS_OS_LINUX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endif(THIS_OS_LINUX)

add_subdirectory(src)
add_subdirectory(app)

if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif(ENABLE_TESTING)

write_register_module_lib()
