#=============================================================================#
# Source group config.
#=============================================================================#
source_group("include" FILES ${BSOID_INCLUDE_TOP_GROUP})
source_group("include\\bsoid" FILES)
source_group("include\\bsoid\\fields" FILES ${BSOID_INCLUDE_FIELDS_GROUP})
source_group("include\\bsoid\\operators" FILES
    ${BSOID_INCLUDE_OPERATORS_GROUP})
source_group("include\\bsoid\\tree" FILES ${BSOID_INCLUDE_TREE_GROUP})
source_group("include\\bsoid\\polygonizer" FILES 
    ${BSOID_INCLUDE_POLYGONIZER_GROUP})
source_group("include\\bsoid\\visualizer" FILES
    ${BSOID_INCLUDE_VISUALIZER_GROUP})
source_group("include\\bsoid\\models" FILES ${BSOID_INCLUDE_MODELS_GROUP})

source_group("source" FILES ${BSOID_SOURCE_TOP_GROUP})
source_group("source\\bsoid" FILES)
source_group("source\\bsoid\\tree" FILES ${BSOID_SOURCE_TREE_GROUP})
source_group("source\\bsoid\\polygonizer" FILES 
    ${BSOID_SOURCE_POLYGONIZER_GROUP})
source_group("source\\bsoid\\visualizer" FILES
    ${BSOID_SOURCE_VISUALIZER_GROUP})
source_group("source\\bsoid\\models" FILES ${BSOID_SOURCE_MODELS_GROUP})

source_group("shader" FILES ${BSOID_SHADER_TOP_GROUP})
source_group("shader\\bsoid" FILES)
source_group("shader\\bsoid\\global" FILES ${BSOID_SHADER_GLOBAL_GROUP})
source_group("shader\\bsoid\\visualizer" FILES
    ${BSOID_SHADER_VISUALIZER_GROUP})
