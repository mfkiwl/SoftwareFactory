CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/evnode)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/basenode)

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
    BpNode.hpp
    BpNode.cpp
    BpNodeFunc.hpp
    BpNodeFunc.cpp
    BpGraph.hpp
    BpGraph.cpp
    Bp.hpp
    Bp.cpp
)

ADD_LIBRARY(bpcore SHARED ${BP_CORE_SRC})
TARGET_LINK_LIBRARIES(bpcore bpcommon bppb)

set(bpcore_headers
    BpContents.hpp
    BpModule.hpp
    BpVariable.hpp
    BpPin.hpp
    BpNode.hpp
)
INSTALL(FILES 
    ${bpcore_headers}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${SF_INSTALL_INC_DIR}
)

INSTALL(TARGETS bpcore
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)

AUX_SOURCE_DIRECTORY(test bpcore_files)
foreach(ut_file ${bpcore_files})
    GET_FILENAME_COMPONENT(ut_file_target ${ut_file} NAME_WE)
    ADD_EXECUTABLE(${ut_file_target} ${ut_file})
    TARGET_LINK_LIBRARIES(${ut_file_target}
        GTest::GTest
        GTest::Main
        bppb
        bpcore
        bpcommon
    )
    add_test(AllTests ${ut_file_target})
endforeach(ut_file ${bpcore_files})

FILE(GLOB test_data_files "test_data/*")
FILE(COPY ${test_data_files} DESTINATION ${SF_OUTPUT_TEST_DATA_DIR})
