project(app)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")

# Add source files for the executable
add_executable(Myapp ${SOURCE_FILES})

if(TRY_PACKAGE_CONFIG)
    message(STATUS "Try using the package config")
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/build/install/lib/cmake)
    find_package(games REQUIRED)
    find_package(calculator REQUIRED)
    target_link_libraries(Myapp PRIVATE magic_enum games::games calculator::calculator)
    target_include_directories(Myapp PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
else(TRY_PACKAGE_CONFIG)
    target_link_libraries(Myapp PRIVATE common magic_enum)
    # Include directories
    target_include_directories(
        Myapp PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/games>
        )
endif(TRY_PACKAGE_CONFIG)

target_set_warnings(TARGET Myapp ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS})

install(TARGETS Myapp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Myapp)
