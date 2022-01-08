project(imgui_node_editor)

find_package(imgui REQUIRED)

add_library(imgui_canvas STATIC
    imgui_canvas.cpp
    imgui_canvas.h
)

target_link_libraries(imgui_canvas PUBLIC imgui)

set(imgui_node_editor_SRC
    imgui_node_editor.h
    crude_json.cpp
    crude_json.h
    imgui_bezier_math.h
    imgui_bezier_math.inl
    imgui_canvas.cpp
    imgui_canvas.h
    imgui_extra_math.h
    imgui_extra_math.inl
    imgui_node_editor_api.cpp
    imgui_node_editor_internal.h
    imgui_node_editor_internal.inl
    imgui_node_editor.cpp
    imgui_node_editor.natvis

    # blueprint
    Interop.h
    ax/Builders.h
    ax/Drawing.h
    ax/Widgets.h
    ax/Math2D.h
    Math2D.inl
    Builders.cpp
    Drawing.cpp
    Widgets.cpp
)

add_library(${PROJECT_NAME} STATIC ${imgui_node_editor_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC . ax)

target_link_libraries(${PROJECT_NAME} PUBLIC imgui)