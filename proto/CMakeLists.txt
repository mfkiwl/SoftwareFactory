CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FILE(GLOB proto_files "*.proto")
PROTOBUF_GENERATE(
    APPEND_PATH
    LANGUAGE cpp
    OUT_VAR pb_gen
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    PROTOS ${proto_files}
)
ADD_LIBRARY(bp_pb SHARED ${pb_gen})
TARGET_LINK_LIBRARIES(bp_pb protobuf)

FILE(GLOB proto_header_files "${CMAKE_CURRENT_BINARY_DIR}/*.pb.h")
INSTALL(FILES ${proto_header_files} DESTINATION usr/local/itools/inc)
INSTALL(TARGETS bp_pb
    LIBRARY DESTINATION usr/local/itools/lib
    ARCHIVE DESTINATION usr/local/itools/lib
    RUNTIME DESTINATION usr/local/itools/bin
)