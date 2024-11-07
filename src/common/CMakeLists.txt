project(common)

find_package(mongocxx REQUIRED)

link_libraries(mongo::mongocxx_shared)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.hpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

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

if(THIS_OS_WINDOWS)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND cmake -E copy_directory_if_different ${PREBUILD_MONGOCXX_DIR}/bin
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin
        COMMAND cmake -E copy_directory_if_different ${PREBUILD_MONGOCXX_DIR}/lib
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../lib
        )
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_targets
        RUNTIME_DEPENDENCIES
        PRE_INCLUDE_REGEXES
        "mongo*"
        "bson*"
        PRE_EXCLUDE_REGEXES
        "api-ms-"
        "ext-ms-"
        POST_EXCLUDE_REGEXES
        ".*system32/.*\\.dll"
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                      COMPONENT PublicHeader
        )
endif()

if(THIS_OS_LINUX)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                      COMPONENT PublicHeader
        )
    find_package(mongoc-1.0 REQUIRED)
    find_package(bsoncxx REQUIRED)
    install(
        IMPORTED_RUNTIME_ARTIFACTS mongo::mongocxx_shared mongo::mongoc_shared mongo::bson_shared
        mongo::bsoncxx_shared
        )
endif()

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
