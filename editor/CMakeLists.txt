CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(SoftwareFactoryEditor)

if (POLICY CMP0072)
  set(OpenGL_GL_PREFERENCE LEGACY)
  # set(OpenGL_GL_PREFERENCE GLVND)
endif()
add_subdirectory(imgui-node-editor)
add_subdirectory(nativefiledialog)
include_directories(nativefiledialog/src/include)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(imgui REQUIRED)
include_directories(${imgui_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/3rd/glew-cmake-2.2.0/include)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor)
include_directories(${GLEW_INCLUDE_DIRS})

set(editor_srcs
    ${CMAKE_SOURCE_DIR}/3rd/imgui/examples/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/3rd/imgui/examples/imgui_impl_sdl.cpp
    SFEditor.cpp
    SFEPanel.cpp
    SFEPanelMainMenu.cpp
    SFEPanelBp.cpp
    SFEPanelLib.cpp
    SFEPanelDragTip.cpp
    SFEPanelGraph.cpp
    SFEPanelLog.cpp
    SFEPanelUINodes.cpp
    SFEPanelPlot.cpp
    SFEPlug.cpp
)

add_library(sfeui SHARED 
    ${editor_srcs}
)

target_compile_definitions(sfeui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(sfeui
    SDL2
    SDL2main
    OpenGL
    GL
    ${GLEW_LIBRARIES}

    bppb
    bpcore
    bpcommon

    imgui
    imgui_node_editor
    nativefiledialog
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
    OpenGL
    GL
    ${GLEW_LIBRARIES}

    bppb
    bpcore
    bpcommon

    imgui
    imgui_node_editor
    nativefiledialog
    sfeui
)

set(sfe_headers
    SFEPanel.hpp
)
INSTALL(FILES 
    ${sfe_headers}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${SF_INSTALL_INC_DIR}
)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR})
INSTALL(TARGETS sfeui
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)

FILE(GLOB assets_files "assets/*")
INSTALL(FILES 
    ${assets_files}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${SF_INSTALL_ASSETS_DIR}
)

FILE(COPY ${assets_files} DESTINATION ${SF_OUTPUT_ASSETS_DIR})