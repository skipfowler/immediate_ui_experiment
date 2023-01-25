#pragma once

#include <GLFW/glfw3.h>
#include <RmlUi/Core/Context.h>

#include "RmlUi_Backend.h"
#include "RmlUi_Platform_GLFW.h"
#include "RmlUi_Renderer_GL3.h"

namespace RmlUi::Backend {
struct DataGlfwGL3 {
  SystemInterface_GLFW system_interface;
  RenderInterface_GL3 render_interface;
  GLFWwindow* window = nullptr;
  int glfw_active_modifiers = 0;
  bool context_dimensions_dirty = true;

  // Arguments set during event processing and nulled otherwise.
  Rml::Context* context = nullptr;
  KeyDownCallback key_down_callback = nullptr;
};
}  // namespace RmlUi::Backend