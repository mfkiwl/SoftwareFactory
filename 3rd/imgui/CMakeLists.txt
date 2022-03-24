set(_imgui_Sources
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    # 3rd
    imgui_plot.cpp
)
source_group("" FILES ${_imgui_Sources})
add_library(imgui SHARED ${_imgui_Sources})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(imgui PUBLIC "ImDrawIdx=unsigned int")
set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")

INSTALL(TARGETS imgui
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)

set(imgui_headers 
    imgui.h
    imconfig.h
)
INSTALL(FILES 
    ${imgui_headers}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${SF_INSTALL_INC_DIR}
)