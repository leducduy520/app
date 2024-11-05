function(REGISTER_MODULE_LIB moduleid)
    message("Register module library module: ${moduleid}, path: ${moduleid}")
    target_compile_definitions(${moduleid} PUBLIC NAME="${moduleid}")
    get_property(module_setting GLOBAL PROPERTY LIST_REGISTER_MODULE)
    set_property(
        GLOBAL PROPERTY LIST_REGISTER_MODULE
                        "${module_setting} \n\tREGISTER_MODULE_LOCATION(${moduleid}, ${moduleid});"
        )

    string(TOUPPER "${moduleid}" moduleid_upper)
    get_property(module_enum GLOBAL PROPERTY LIST_REGISTER_MODULE_ENUM)
    if(module_enum)
        string(PREPEND moduleid_upper ",\n\t")
    else()
        string(PREPEND moduleid_upper "\t")
    endif()
    set_property(GLOBAL PROPERTY LIST_REGISTER_MODULE_ENUM "${module_enum} ${moduleid_upper}")
endfunction()

function(WRITE_REGISTER_MODULE_LIB)
    get_property(module_setting GLOBAL PROPERTY LIST_REGISTER_MODULE)
    get_property(module_enum GLOBAL PROPERTY LIST_REGISTER_MODULE_ENUM)
    message("LIST_REGISTER_MODULE : ${module_setting}")
    set(register_lib "${module_setting}")
    set(register_enum "${module_enum}")
    configure_file(${PRE_DEFINITION_PATH}.in ${PRE_DEFINITION_PATH})
endfunction()

macro(regist_module module_name)
    cmake_path(
        RELATIVE_PATH CMAKE_CURRENT_LIST_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE
        REL_PATH
        )
    set(HEADERS_INCLUDE_DIRS ${CMAKE_INSTALL_INCLUDEDIR}/${REL_PATH})

    file(GLOB_RECURSE SOURCE_FILES "*.cpp")
    file(GLOB_RECURSE HEADER_FILES "*.hpp")
    file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

    # message("SOURCE_FILES: ${SOURCE_FILES}")
    # message("HEADER_FILES: ${HEADER_FILES}")
    # Filter to keep only directories
    set(DIR_LIST "")
    foreach(subdir ${subdirectories})
        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
            list(APPEND DIR_LIST ${subdir})
        endif()
    endforeach()

    # Print all subdirectory names
    # message(STATUS "Subdirectories:")
    # foreach(dir ${DIR_LIST})
    #     message(STATUS " - ${dir}")
    # endforeach()

    add_library(${module_name} SHARED ${SOURCE_FILES})
    target_link_libraries(${module_name} PRIVATE common)
    target_include_directories(
        ${module_name}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> $<BUILD_INTERFACE:${DIR_LIST}>
               $<INSTALL_INTERFACE:${HEADERS_INCLUDE_DIRS}>
        )

    if(THIS_COMPILER_GCC OR THIS_COMPILER_CLANG)
        set_target_properties(${module_name} PROPERTIES PREFIX "")
    endif()

    install(
        TARGETS ${module_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        PUBLIC_HEADER DESTINATION ${HEADERS_INCLUDE_DIRS} COMPONENT PublicHeader
        )

    register_module_lib(${module_name})
endmacro(regist_module module_name)
