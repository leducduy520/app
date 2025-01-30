function(register_module_lib moduleid)
    if(THIS_OS_WINDOWS)
        set(DYM_LIB_EXPANSION ".dll")
    elseif(THIS_OS_LINUX)
        set(DYM_LIB_EXPANSION ".so")
    endif()

    message("Register module library module: ${moduleid}, path: ${moduleid}${DYM_LIB_EXPANSION}")
    target_compile_definitions(${moduleid} PUBLIC NAME="${moduleid}")
    get_property(registed_module_list GLOBAL PROPERTY LIST_REGISTED_MODULE)
    set_property(
        GLOBAL
        PROPERTY LIST_REGISTED_MODULE
                 "${registed_module_list} \n\tREGISTER_MODULE_LOCATION(${moduleid}, ${moduleid});"
        )

    string(TOUPPER "${moduleid}" moduleid_upper)
    get_property(registed_enum_list GLOBAL PROPERTY LIST_REGISTED_MODULE_ENUM)

    if(registed_enum_list)
        string(PREPEND moduleid_upper ",\n\t")
    else()
        string(PREPEND moduleid_upper "\t")
    endif()

    set_property(
        GLOBAL PROPERTY LIST_REGISTED_MODULE_ENUM "${registed_enum_list} ${moduleid_upper}"
        )
endfunction()

function(write_registed_module_lib)
    get_property(registed_module_list GLOBAL PROPERTY LIST_REGISTED_MODULE)
    get_property(registed_enum_list GLOBAL PROPERTY LIST_REGISTED_MODULE_ENUM)
    message("LIST_REGISTED_MODULE : ${registed_module_list}")
    set(register_lib "${registed_module_list}")
    set(register_enum "${registed_enum_list}")
    configure_file(${PRE_DEFINITION_PATH}.in ${PRE_DEFINITION_PATH})
endfunction()

macro(regist_module module_name)
    cmake_path(
        RELATIVE_PATH CMAKE_CURRENT_LIST_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE
        REL_PATH
        )

    file(GLOB_RECURSE SOURCE_FILES "*.cpp")
    file(GLOB_RECURSE HEADER_FILES "*.hpp")
    file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

    message(DEBUG "SOURCE_FILES: ${SOURCE_FILES}")
    message(DEBUG "HEADER_FILES: ${HEADER_FILES}")

    # Filter to keep only directories
    set(DIR_LIST "")

    foreach(subdir ${subdirectories})
        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
            list(APPEND DIR_LIST ${subdir})
        endif()
    endforeach()

    # Print all subdirectory names
    message(DEBUG "Subdirectories:")

    foreach(dir ${DIR_LIST})
        message(DEBUG " - ${dir}")
    endforeach()

    add_library(${module_name} SHARED ${SOURCE_FILES})
    target_link_libraries(${module_name} PRIVATE common)
    target_include_directories(
        ${module_name} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
                               $<BUILD_INTERFACE:${DIR_LIST}>
        )

    if(THIS_COMPILER_GCC OR THIS_COMPILER_CLANG)
        set_target_properties(${module_name} PROPERTIES PREFIX "")
    endif()

    register_module_lib(${module_name})

    install(
        TARGETS ${module_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        )
endmacro(regist_module module_name)
