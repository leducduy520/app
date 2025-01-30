project(common)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.hpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)
find_package(mongocxx REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(nlohmann_json REQUIRED)
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        nlohmann_json::nlohmann_json cpprestsdk::cpprest
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_shared>,mongo::mongocxx_shared,mongo::mongocxx_static>
    )

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

if(ENABLE_SANITIZE_ADDR
   OR ENABLE_SANITIZE_UNDEF
   OR ENABLE_SANITIZE_LEAK
   OR ENABLE_SANITIZE_THREAD
   )
    include(Sanitizer)
    directory_add_sanitizer(
        ${ENABLE_SANITIZE_ADDR} ${ENABLE_SANITIZE_UNDEF} ${ENABLE_SANITIZE_LEAK}
        ${ENABLE_SANITIZE_THREAD}
        )
endif()

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
