function(build_html_document BASEDIR)
    find_package(Doxygen QUIET COMPONENTS dot doxygen)

    if(NOT Doxygen_FOUND)
        return()
    endif()

    add_custom_target(
        gen_doc
        COMMAND ${DOXYGEN_EXECUTABLE} ./docCfg
        COMMENT "Generate project html document."
        WORKING_DIRECTORY ${BASEDIR}
    )
    set_target_properties(gen_doc PROPERTIES FOLDER "Document")
endfunction()

function(build_dependencies_graph BASEDIR NAME)
    add_custom_command(
        OUTPUT ${BASEDIR}/${NAME}.png
        COMMAND ${DOXYGEN_DOT_EXECUTABLE} -v -Tpng ${BASEDIR}/graph.dot -o ${BASEDIR}/${NAME}.png
        WORKING_DIRECTORY ${BASEDIR}
        VERBATIM USES_TERMINAL
    )

    add_custom_target(
        gen_graph COMMENT "Generate project dependencies graph."
        DEPENDS ${BASEDIR}/${NAME}.png
    )
    set_target_properties(gen_graph PROPERTIES FOLDER "Document")
endfunction()
