project(app)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")

# Add source files for the executable
add_executable(Myapp ${SOURCE_FILES})
target_link_libraries(Myapp PRIVATE magic_enum)
# Include directories
target_include_directories(
    Myapp
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/games>
)

# Link the GameManager library and SFML libraries
# target_link_libraries(MySFMLApp PRIVATE sfml-graphics sfml-window sfml-system)
target_set_warnings(
    TARGET Myapp ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
    )

install(
    TARGETS Myapp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    )

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Myapp)
