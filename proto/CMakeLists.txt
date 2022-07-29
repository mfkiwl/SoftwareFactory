CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

IF(USE_PB_VER_2)
    MESSAGE(STATUS "Using protobuf2 proto...")
    FILE(GLOB proto_files "pb2/*.proto")
ELSE()
    MESSAGE(STATUS "Using protobuf3 proto...")
    FILE(GLOB proto_files "*.proto")
ENDIF()

include_directories(${Protobuf_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto_files})
ADD_LIBRARY(bppb SHARED ${PROTO_SRCS} ${PROTO_HDRS})
TARGET_LINK_LIBRARIES(bppb ${Protobuf_LIBRARIES})

FILE(GLOB proto_header_files "${CMAKE_CURRENT_BINARY_DIR}/*.pb.h")
INSTALL(FILES ${proto_header_files} DESTINATION ${SF_INSTALL_INC_DIR})
INSTALL(TARGETS bppb
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)