project(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME}
  VERSION ${SOLUTION_VERSION}
  LANGUAGES CXX
)

add_library(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME} STATIC
  "imgui_impl_glfw.cpp"
  "imgui_impl_glfw.h"
  "imgui_impl_opengl3.cpp"
  "imgui_impl_opengl3.h"
  "imgui_impl_opengl3_loader.h"
)

target_link_libraries(
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME}
  CONAN_PKG::glfw
  CONAN_PKG::imgui
)