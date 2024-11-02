project(games)

file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

message("SOURCE_FILES: ${SOURCE_FILES}")
message("HEADER_FILES: ${HEADER_FILES}")
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

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE common)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/app ${CMAKE_CURRENT_LIST_DIR} ${DIR_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "game_interface.h")
install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} COMPONENT PublicHeader
)
