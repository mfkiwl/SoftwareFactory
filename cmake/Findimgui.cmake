set(imgui_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd/imgui)
set(imgui_INCLUDE_DIRS 
    ${imgui_INCLUDE_DIR}
    ${imgui_INCLUDE_DIR}/examples
)

set(imgui_LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libimgui.so)
set(imgui_LIBRARYS "${imgui_LIBRARY}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    imgui
    DEFAULT_MSG
        imgui_INCLUDE_DIR
        imgui_LIBRARY
        imgui_LIBRARYS
)

mark_as_advanced(imgui_INCLUDE_DIR imgui_LIBRARY imgui_LIBRARYS)