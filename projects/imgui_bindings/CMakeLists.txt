project(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME}
  VERSION ${SOLUTION_VERSION}
  LANGUAGES CXX
)

set(PROJECTS_IMGUI_BINDINGS_FILES
  "imgui_impl_glfw.cpp"
  "imgui_impl_glfw.h"
  "imgui_impl_opengl3.cpp"
  "imgui_impl_opengl3.h"
  "imgui_impl_opengl3_loader.h"
)

add_library(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME} STATIC
  ${PROJECTS_IMGUI_BINDINGS_FILES}
)

target_link_libraries(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME}
  CONAN_PKG::glfw
  CONAN_PKG::imgui
)

source_group(
  TREE ${CMAKE_CURRENT_LIST_DIR} 
  FILES ${PROJECTS_IMGUI_BINDINGS_FILES}
)

set_target_properties(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME} PROPERTIES
  FOLDER "projects"
)