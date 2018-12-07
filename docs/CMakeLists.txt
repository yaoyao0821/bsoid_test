add_custom_target(bsoid_docs ALL ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${BSOID_BINARY_DIR}/docs
    COMMENT "Generating HTML documentation" VERBATIM)
set_target_properties(bsoid_docs PROPERTIES FOLDER "bsoid")
