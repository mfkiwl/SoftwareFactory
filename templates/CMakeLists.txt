CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(TEMPLATE_SRC
	main.cpp
    TemplateFlags.cpp
)
add_executable(bptemplate ${TEMPLATE_SRC})
target_link_libraries(bptemplate
    bpcommon
    bplog
    bppb
    bpcore
)

INSTALL(DIRECTORY base_mod DESTINATION ${SF_INSTALL_TEMPLATES_DIR})

FILE(GLOB bptemplates_headers "*.hpp")
INSTALL(FILES 
    ${bptemplates_headers}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${SF_INSTALL_INC_DIR}
)

INSTALL(TARGETS bptemplate
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)