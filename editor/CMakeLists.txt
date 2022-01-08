add_subdirectory(imgui-node-editor)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(imgui REQUIRED)
include_directories(${imgui_INCLUDE_DIRS})

set(editor_srcs
    main.cpp
)

add_executable(SoftwareFactoryEditor ${editor_srcs})
target_link_libraries(SoftwareFactoryEditor 
    ${GLEW_LIBRARIES} 
    ${SDL2_LIBRARIES}
    OpenGL
    imgui 
    imgui_canvas
    imgui_node_editor
)