function(add_project_documentation)
    find_package(Doxygen REQUIRED COMPONENTS dot doxygen)

    if(NOT Doxygen_FOUND)
        return()
    endif()
    
    set(DOCS_DIR ${CMAKE_SOURCE_DIR}/docs)
    set(DOCS_HTML_DIR ${DOCS_DIR}/html CACHE PATH "project html directory")
    set(DOCS_GRAPH_DIR ${DOCS_DIR}/graph CACHE PATH "project graph directory")

    if(BUILD_HIERARCHY_GRAPH)
        add_custom_command(
            OUTPUT ${DOCS_GRAPH_DIR}/mysfmlapp-graph.png
            COMMAND ${DOXYGEN_DOT_EXECUTABLE} -v -Tpng graph.dot -o mysfmlapp-graph.png
            WORKING_DIRECTORY ${DOCS_GRAPH_DIR}
            VERBATIM USES_TERMINAL
        )

        add_custom_target(
            gen_graph COMMENT "Generate project graph dependencies"
            DEPENDS ${DOCS_GRAPH_DIR}/mysfmlapp-graph.png
        )
        set_target_properties(gen_graph PROPERTIES FOLDER "Custom target")
    endif(BUILD_HIERARCHY_GRAPH)

    if(BUILD_WEB_DOC)
        add_custom_target(
            gen_doc
            COMMAND ${DOXYGEN_EXECUTABLE} ./docCfg
            COMMENT "Generate project document"
            WORKING_DIRECTORY ${DOCS_DIR}
        )
        set_target_properties(gen_doc PROPERTIES FOLDER "Custom target")
    endif(BUILD_WEB_DOC)

    if(INSTALL_HIERARCHY_GRAPH)
        install(
            FILES ${DOCS_GRAPH_DIR}/mysfmlapp-graph.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/MySFMLApp
        )
    endif(INSTALL_HIERARCHY_GRAPH)

    if(INSTALL_WEB_DOC)
        install(DIRECTORY ${DOCS_HTML_DIR} DESTINATION ${CMAKE_INSTALL_DATADIR}/MySFMLApp)
    endif(INSTALL_WEB_DOC)
endfunction()
