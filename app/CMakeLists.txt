project(app)

# Set custom variables
set(PRE_DEFINITION_PATH ${CMAKE_SOURCE_DIR}/app/pre-definition.hpp)

# Generate position-independent code (-fPIC on UNIX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(calculator)
add_subdirectory(games)
add_subdirectory(translate)

write_registed_module_lib()

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")

# Add source files for the executable
add_executable(Myapp ${SOURCE_FILES})

target_link_libraries(Myapp PRIVATE common)
target_include_directories(
    Myapp PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_set_warnings(TARGET Myapp ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS})

install(TARGETS Myapp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Myapp)
