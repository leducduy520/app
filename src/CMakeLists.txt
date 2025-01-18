project(src)

if(THIS_COMPILER_GCC)
    add_compile_options(-fPIC)
endif(THIS_COMPILER_GCC)

add_subdirectory(common)
add_subdirectory(games)
add_subdirectory(calculator)
add_subdirectory(translate)
write_registed_module_lib()
