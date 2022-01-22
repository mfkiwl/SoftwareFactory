CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
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
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(editor_srcs
    ${CMAKE_SOURCE_DIR}/3rd/imgui/examples/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/3rd/imgui/examples/imgui_impl_sdl.cpp
    main.cpp
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
)
add_executable(SoftwareFactoryEditor ${editor_srcs})
target_compile_definitions(SoftwareFactoryEditor PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(SoftwareFactoryEditor
    SDL2
    SDL2main
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libglew.a
    OpenGL
    GL
    
    nativefiledialog
    imgui 
    imgui_canvas
    imgui_node_editor
    
    bp_pb
    bpcore
    bpcommon
)

FILE(GLOB assets_files "assets/*")
INSTALL(FILES 
    ${assets_files}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${ITOOLS_INSTALL_ASSETS_DIR}
)

FILE(COPY ${assets_files} DESTINATION "${CMAKE_BINARY_DIR}/build/assets/")