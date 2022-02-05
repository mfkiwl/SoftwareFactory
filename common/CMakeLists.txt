CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_SUBDIRECTORY(bplog)

ADD_LIBRARY(bpcommon SHARED
    bpcommon.cpp
)
TARGET_LINK_LIBRARIES(bpcommon)

INSTALL(TARGETS bpcommon
    LIBRARY DESTINATION ${ITOOLS_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${ITOOLS_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${ITOOLS_INSTALL_BIN_DIR}
)

AUX_SOURCE_DIRECTORY(test bpcommon_files)
foreach(ut_file ${bpcommon_files})
    GET_FILENAME_COMPONENT(ut_file_target ${ut_file} NAME_WE)
    ADD_EXECUTABLE(${ut_file_target} ${ut_file})
    TARGET_LINK_LIBRARIES(${ut_file_target}
        gtest_main    
        gtest
        bp_pb
        pthread
        bpcommon
        jsoncpp
    )
endforeach(ut_file ${bpcommon_files})

