project(games)
regist_module(${PROJECT_NAME})

# write_basic_package_version_file(
#     ${PROJECT_NAME}-config-version.cmake VERSION 0.0.1 COMPATIBILITY SameMajorVersion
#     )

# configure_file(${PROJECT_NAME}-config.cmake.in ${PROJECT_NAME}-config.cmake @ONLY)

# install(
#     TARGETS ${PROJECT_NAME}
#     EXPORT ${PROJECT_NAME}_targets
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
#     )

# install(
#     EXPORT ${PROJECT_NAME}_targets
#     NAMESPACE ${PROJECT_NAME}::
#     FILE ${PROJECT_NAME}-targets.cmake
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#     )

# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
#           ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} COMPONENT Config
#     )
