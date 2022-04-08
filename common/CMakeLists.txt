CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

ADD_LIBRARY(bpcommon SHARED
    bpcommon.cpp
    bpflags.cpp
    bplog/bplog.cpp
    bplog/bplogger.cpp
)
TARGET_LINK_LIBRARIES(bpcommon)

FILE(GLOB bpcommon_headers "*.hpp")
INSTALL(FILES 
    ${bpcommon_headers}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${SF_INSTALL_INC_DIR}
)

INSTALL(TARGETS bpcommon
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)

AUX_SOURCE_DIRECTORY(test bpcommon_files)
foreach(ut_file ${bpcommon_files})
    GET_FILENAME_COMPONENT(ut_file_target ${ut_file} NAME_WE)
    ADD_EXECUTABLE(${ut_file_target} ${ut_file})
    TARGET_LINK_LIBRARIES(${ut_file_target}
        GTest::GTest
        GTest::Main
        bppb
        pthread
        bpcommon
        jsoncpp
    )
    add_test(AllTests ${ut_file_target})
endforeach(ut_file ${bpcommon_files})

