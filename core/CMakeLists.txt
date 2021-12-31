CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(BP_CORE_SRC
	BpContents.hpp
	BpModule.hpp
	BpModule.cpp
    BpModuleLinux.hpp
    BpModuleLinux.cpp
    BpModLib.hpp
    BpModLibLinux.hpp
    BpModLibLinux.cpp
    BpNodeLib.hpp
    BpNodeLib.cpp

    BpVariable.hpp
    BpVariable.cpp
    BpLink.hpp
    BpPin.hpp
    BpPin.cpp
    BpObj.hpp
    BpObj.cpp
    BpNode.hpp
    BpNode.cpp
    BpGraph.hpp
    BpGraph.cpp
    Bp.hpp
    Bp.cpp
)

ADD_LIBRARY(bpcore SHARED ${BP_CORE_SRC})
TARGET_LINK_LIBRARIES(bpcore bpcommon)

INSTALL(TARGETS bpcore
    LIBRARY DESTINATION ${ITOOLS_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${ITOOLS_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${ITOOLS_INSTALL_BIN_DIR}
)

AUX_SOURCE_DIRECTORY(test bpcore_files)
foreach(ut_file ${bpcore_files})
    GET_FILENAME_COMPONENT(ut_file_target ${ut_file} NAME_WE)
    ADD_EXECUTABLE(${ut_file_target} ${ut_file})
    TARGET_LINK_LIBRARIES(${ut_file_target}
        gtest_main    
        gtest
        bp_pb
        bpcore
        bpcommon
    )
endforeach(ut_file ${bpcore_files})
