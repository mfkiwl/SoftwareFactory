find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(_imgui_Sources
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    examples/imgui_impl_opengl3.cpp
    examples/imgui_impl_sdl.cpp
)
source_group("" FILES ${_imgui_Sources})
add_library(imgui STATIC ${_imgui_Sources})
target_link_libraries(imgui ${SDL2_LIBRARIES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(imgui PUBLIC "ImDrawIdx=unsigned int")
set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")