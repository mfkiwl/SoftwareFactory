set(_imgui_Sources
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
)
source_group("" FILES ${_imgui_Sources})
add_library(imgui STATIC ${_imgui_Sources})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(imgui PUBLIC "ImDrawIdx=unsigned int")
set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")