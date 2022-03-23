project(nativefiledialog)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories (${GTK3_INCLUDE_DIRS})
link_directories (${GTK3_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
add_library(${PROJECT_NAME} SHARED
    src/nfd_common.c
    src/nfd_gtk.c
)
add_definitions(${GTK3_CFLAGS_OTHER})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${SF_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${SF_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${SF_INSTALL_BIN_DIR}
)