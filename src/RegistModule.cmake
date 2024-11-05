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
    if(THIS_COMPILER_GCC)
        target_compile_options(${module_name} PUBLIC -fPIC)
    endif()

    install(
        TARGETS ${module_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        PUBLIC_HEADER DESTINATION ${HEADERS_INCLUDE_DIRS} COMPONENT PublicHeader
        )

    register_module_lib(${module_name})
endmacro(regist_module module_name)
