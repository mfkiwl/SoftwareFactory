CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(SoftwareFactoryEditor)

if (POLICY CMP0072)
  set(OpenGL_GL_PREFERENCE LEGACY)
  # set(OpenGL_GL_PREFERENCE GLVND)
endif()
add_subdirectory(imgui-node-editor)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(imgui REQUIRED)
include_directories(${imgui_INCLUDE_DIRS})

set(editor_srcs
    ${CMAKE_SOURCE_DIR}/3rd/imgui/examples/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/3rd/imgui/examples/imgui_impl_sdl.cpp
    main.cpp
    SFEditor.cpp
    SFEPanel.cpp
)
add_executable(SoftwareFactoryEditor ${editor_srcs})
target_compile_definitions(SoftwareFactoryEditor PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(SoftwareFactoryEditor
    SDL2
    SDL2main
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libglew.a
    OpenGL
    GL
    imgui 
    imgui_canvas
    imgui_node_editor
)