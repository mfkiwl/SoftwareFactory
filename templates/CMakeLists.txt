CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(TEMPLATE_SRC
	main.cpp
    TemplateFlags.cpp
)
add_executable(template ${TEMPLATE_SRC})
target_link_libraries(template
    bpcommon
    bplog
    bppb
    bpcore
)
INSTALL(TARGETS template
    LIBRARY DESTINATION ${ITOOLS_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${ITOOLS_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${ITOOLS_INSTALL_BIN_DIR}
)