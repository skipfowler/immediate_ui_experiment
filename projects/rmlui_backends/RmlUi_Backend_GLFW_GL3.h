#pragma once

#include <GLFW/glfw3.h>
#include <RmlUi/Core/Context.h>


#include "RmlUi_Backend.h"
#include "RmlUi_Backend_GLFW_GL3_Data.h"
#include "RmlUi_Platform_GLFW.h"
#include "RmlUi_Renderer_GL3.h"

namespace RmlUi::Backend {
class glfwGL3 : BackendInterface {
 public:
  bool Initialize(const char* name, int width, int height, bool allow_resize);
  void Shutdown();
  Rml::SystemInterface* GetSystemInterface();
  Rml::RenderInterface* GetRenderInterface();
  bool ProcessEvents(Rml::Context* context, KeyDownCallback key_down_callback);
  void RequestExit();
  void BeginFrame();
  void PresentFrame();
  void SetBackend(Rml::SharedPtr<DataGlfwGL3> backend);

 private:
  void SetupCallbacks(GLFWwindow* window);

  Rml::SharedPtr<DataGlfwGL3> _backend;
};
}  // namespace RmlUi::Backend