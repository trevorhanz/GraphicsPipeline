find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ../README.md)
    set(DOXYGEN_PREDEFINED __cplusplus)
    
    doxygen_add_docs(gp-docs
        ../README.md
        examples.md
        ${PROJECT_SOURCE_DIR}/include)
endif()
