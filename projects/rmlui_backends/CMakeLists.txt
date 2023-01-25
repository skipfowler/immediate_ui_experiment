project(
  ${PROJECTS_RMLUI_BACKENDS_TARGET_NAME}
  VERSION ${SOLUTION_VERSION}
  LANGUAGES CXX
)

add_library(
  ${PROJECTS_RMLUI_BACKENDS_TARGET_NAME} STATIC
  "RmlUi_Backend.h"
  "RmlUi_Backend_GLFW_GL3.cpp"
  "RmlUi_Include_GL3.h"
  "RmlUi_Platform_GLFW.cpp"
  "RmlUi_Platform_GLFW.h"
  "RmlUi_Renderer_GL3.cpp"
  "RmlUi_Renderer_GL3.h"
)

target_link_libraries(
  ${PROJECTS_RMLUI_BACKENDS_TARGET_NAME}
  CONAN_PKG::glfw
  CONAN_PKG::rmlui
)