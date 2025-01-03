project(app)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")

# Add source files for the executable
add_executable(Myapp ${SOURCE_FILES})

target_link_libraries(Myapp PRIVATE common)
target_include_directories(
    Myapp PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/games>
    )

target_set_warnings(TARGET Myapp ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS})

install(TARGETS Myapp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Myapp)
