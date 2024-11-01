project(app)

# Add source files for the executable
add_executable(Myapp main.cpp module_manager.cpp)

# Include directories
target_include_directories(
    Myapp
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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
