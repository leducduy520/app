macro(add_pkg_config module_name)
    write_basic_package_version_file(
        ${module_name}-config-version.cmake VERSION 0.0.1 COMPATIBILITY SameMajorVersion
        )

    configure_file(${module_name}-config.cmake.in ${module_name}-config.cmake @ONLY)

    install(
        EXPORT ${module_name}_targets
        NAMESPACE ${module_name}::
        FILE ${module_name}-targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${module_name}
        )

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-config-version.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${module_name} COMPONENT Config
        )
endmacro(add_pkg_config module_name)
