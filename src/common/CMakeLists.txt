project(common)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.hpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

find_package(mongocxx REQUIRED)
find_package(cpprestsdk REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC cpprestsdk::cpprest mongo::mongocxx_shared)

# target_link_libraries(${PROJECT_NAME} PUBLIC rapidcsv magic_enum nlohmann_json::nlohmann_json)

target_include_directories(
    ${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    )

target_set_warnings(
    TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
    )

set_target_properties(
    ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}" WINDOWS_EXPORT_ALL_SYMBOLS ON
    )

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} COMPONENT PublicHeader
    )

install(
    EXPORT ${PROJECT_NAME}_targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
    )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake" VERSION 0.0.1
    COMPATIBILITY AnyNewerVersion
    )

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
    )

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
    )
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/prebuild/mongo-cxx-driver/ DESTINATION .)
