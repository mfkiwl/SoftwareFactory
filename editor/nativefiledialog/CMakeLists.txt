project(nativefiledialog)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories (${GTK3_INCLUDE_DIRS})
link_directories (${GTK3_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
add_library(nativefiledialog STATIC
    src/nfd_common.c
    src/nfd_gtk.c
)
add_definitions(${GTK3_CFLAGS_OTHER})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES})