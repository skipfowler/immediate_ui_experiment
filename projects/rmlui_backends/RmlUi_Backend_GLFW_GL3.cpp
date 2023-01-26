#include "RmlUi_Backend_GLFW_GL3.h"

#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Profiling.h>

static void LogErrorFromGLFW(int error, const char* description) {
  Rml::Log::Message(Rml::Log::LT_ERROR, "GLFW error (0x%x): %s", error,
                    description);
}

namespace RmlUi::Backend {
bool glfwGL3::Initialize(const char* name, int width, int height,
                         bool allow_resize) {
  glfwSetErrorCallback(LogErrorFromGLFW);

  if (!glfwInit()) return false;

  // Set window hints for OpenGL 3.3 Core context creation.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  // Request stencil buffer of at least 8-bit size to supporting clipping
  // on transformed elements.
  glfwWindowHint(GLFW_STENCIL_BITS, 8);

  // Enable MSAA for better-looking visuals, especially when transforms
  // are applied.
  glfwWindowHint(GLFW_SAMPLES, 2);

  // Apply window properties and create it.
  glfwWindowHint(GLFW_RESIZABLE, allow_resize ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(width, height, name, nullptr, nullptr);
  if (!window) return false;

  glfwSetWindowUserPointer(window, _backend.get());
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Load the OpenGL functions.
  Rml::String renderer_message;
  if (!RmlGL3::Initialize(&renderer_message)) return false;

  _backend = Rml::MakeShared<DataGlfwGL3>();

  // Construct the system and render interface, this includes compiling
  // all the shaders. If this fails, it is likely an error in the shader
  // code.
  if (!_backend->render_interface) return false;

  _backend->window = window;
  _backend->system_interface.SetWindow(window);
  _backend->system_interface.LogMessage(Rml::Log::LT_INFO, renderer_message);

  // The window size may have been scaled by DPI settings, get the actual
  // pixel size.
  glfwGetFramebufferSize(window, &width, &height);
  _backend->render_interface.SetViewport(width, height);

  // Receive num lock and caps lock modifiers for proper handling of
  // numpad inputs in text fields.
  glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

  // Setup the input and window event callback functions.
  SetupCallbacks(window);

  return true;
}

void glfwGL3::Shutdown() {
  glfwDestroyWindow(_backend->window);
  RmlGL3::Shutdown();
  glfwTerminate();
}

Rml::Context* glfwGL3::CreateContext() {
  if (nullptr == _backend->context) {
    int width, height;
    glfwGetFramebufferSize(_backend->window, &width, &height);
    _backend->context =
        Rml::CreateContext("main", Rml::Vector2i(width, height));
  }

  return _backend->context;
}

Rml::SystemInterface* glfwGL3::GetSystemInterface() {
  return &_backend->system_interface;
}
Rml::RenderInterface* glfwGL3::GetRenderInterface() {
  return &_backend->render_interface;
}

bool glfwGL3::ProcessEvents(Rml::Context* context,
                            KeyDownCallback key_down_callback) {
  // The initial window size may have been affected by system DPI
  // settings, apply the actual pixel size and dp-ratio to the context.
  if (_backend->context_dimensions_dirty) {
    _backend->context_dimensions_dirty = false;

    Rml::Vector2i window_size;
    float dp_ratio = 1.f;
    glfwGetFramebufferSize(_backend->window, &window_size.x, &window_size.y);
    glfwGetWindowContentScale(_backend->window, &dp_ratio, nullptr);

    context->SetDimensions(window_size);
    context->SetDensityIndependentPixelRatio(dp_ratio);
  }

  _backend->context = context;
  _backend->key_down_callback = key_down_callback;

  glfwPollEvents();

  const bool result = !glfwWindowShouldClose(_backend->window);
  glfwSetWindowShouldClose(_backend->window, GLFW_FALSE);
  return result;
}

void glfwGL3::RequestExit() {
  glfwSetWindowShouldClose(_backend->window, GLFW_TRUE);
}

void glfwGL3::BeginFrame() {
  _backend->render_interface.BeginFrame();
  _backend->render_interface.Clear();
}

void glfwGL3::PresentFrame() {
  _backend->render_interface.EndFrame();
  glfwSwapBuffers(_backend->window);

  // Optional, used to mark frames during performance profiling.
  RMLUI_FrameMark;
}

void glfwGL3::SetupCallbacks(GLFWwindow* window) {
  // Key input
  glfwSetKeyCallback(window, [](GLFWwindow* window, int glfw_key, int scancode,
                                int glfw_action, int glfw_mods) {
    auto data =
        reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

    if (nullptr == data) {
      return;
    }

    if (!data->context) {
      return;
    }

    // Store the active modifiers for later because GLFW doesn't provide
    // them in the callbacks to the mouse input events.
    data->glfw_active_modifiers = glfw_mods;

    // Override the default key event callback to add global shortcuts for
    // the samples.
    Rml::Context* context = data->context;
    KeyDownCallback key_down_callback = data->key_down_callback;

    switch (glfw_action) {
      case GLFW_PRESS:
      case GLFW_REPEAT: {
        const Rml::Input::KeyIdentifier key = RmlGLFW::ConvertKey(glfw_key);
        const int key_modifier = RmlGLFW::ConvertKeyModifiers(glfw_mods);
        float dp_ratio = 1.f;
        glfwGetWindowContentScale(data->window, &dp_ratio, nullptr);

        // See if we have any global shortcuts that take priority over the
        // context.
        if (key_down_callback &&
            !key_down_callback(context, key, key_modifier, dp_ratio, true))
          break;
        // Otherwise, hand the event over to the context by calling the
        // input handler as normal.
        if (!RmlGLFW::ProcessKeyCallback(context, glfw_key, glfw_action,
                                         glfw_mods))
          break;
        // The key was not consumed by the context either, try keyboard
        // shortcuts of lower priority.
        if (key_down_callback &&
            !key_down_callback(context, key, key_modifier, dp_ratio, false))
          break;
      } break;
      case GLFW_RELEASE:
        RmlGLFW::ProcessKeyCallback(context, glfw_key, glfw_action, glfw_mods);
        break;
    }
  });

  glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
    auto data =
        reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

    if (nullptr == data) {
      return;
    }

    RmlGLFW::ProcessCharCallback(data->context, codepoint);
  });

  glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
    auto data =
        reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

    if (nullptr == data) {
      return;
    }

    RmlGLFW::ProcessCursorEnterCallback(data->context, entered);
  });

  // Mouse input
  glfwSetCursorPosCallback(
      window, [](GLFWwindow* window, double xpos, double ypos) {
        auto data =
            reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

        if (nullptr == data) {
          return;
        }

        RmlGLFW::ProcessCursorPosCallback(data->context, xpos, ypos,
                                          data->glfw_active_modifiers);
      });

  glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button,
                                        int action, int mods) {
    auto data =
        reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

    if (nullptr == data) {
      return;
    }

    data->glfw_active_modifiers = mods;
    RmlGLFW::ProcessMouseButtonCallback(data->context, button, action, mods);
  });

  glfwSetScrollCallback(
      window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto data =
            reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

        if (nullptr == data) {
          return;
        }

        RmlGLFW::ProcessScrollCallback(data->context, yoffset,
                                       data->glfw_active_modifiers);
      });

  // Window events
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow* window, int width, int height) {
        auto data =
            reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

        if (nullptr == data) {
          return;
        }

        data->render_interface.SetViewport(width, height);
        RmlGLFW::ProcessFramebufferSizeCallback(data->context, width, height);
      });

  glfwSetWindowContentScaleCallback(
      window, [](GLFWwindow* window, float xscale, float yscale) {
        auto data =
            reinterpret_cast<DataGlfwGL3*>(glfwGetWindowUserPointer(window));

        if (nullptr == data) {
          return;
        }

        { RmlGLFW::ProcessContentScaleCallback(data->context, xscale); }
      });
}

Rml::SharedPtr<DataGlfwGL3> glfwGL3::GetBackend() { return _backend; }
}  // namespace RmlUi::Backend