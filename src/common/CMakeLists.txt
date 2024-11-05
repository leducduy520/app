project(common)

cmake_path(
    RELATIVE_PATH CMAKE_CURRENT_LIST_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE
    REL_PATH
    )
set(HEADERS_INCLUDE_DIRS ${CMAKE_INSTALL_INCLUDEDIR}/${REL_PATH})

find_package(mongocxx REQUIRED)

link_libraries(mongo::mongocxx_shared)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.hpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC rapidcsv magic_enum nlohmann_json::nlohmann_json)

target_include_directories(
    ${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${HEADERS_INCLUDE_DIRS}>
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
        TARGETS
            ${PROJECT_NAME}
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
        PUBLIC_HEADER DESTINATION ${HEADERS_INCLUDE_DIRS} COMPONENT PublicHeader
        )
endif()

if(THIS_OS_LINUX)
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        PUBLIC_HEADER DESTINATION ${HEADERS_INCLUDE_DIRS} COMPONENT PublicHeader
        )
    find_package(mongoc-1.0 REQUIRED)
    find_package(bsoncxx REQUIRED)
    install(
        IMPORTED_RUNTIME_ARTIFACTS mongo::mongocxx_shared mongo::mongoc_shared mongo::bson_shared
        mongo::bsoncxx_shared
        )
endif()

# install(DIRECTORY ${CMAKE_SOURCE_DIR}/prebuild/mongo-cxx-driver/ DESTINATION .)
