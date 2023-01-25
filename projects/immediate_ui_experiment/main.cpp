#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include "DemoEventListener.h"
#include "DemoEventListenerInstancer.h"
#include "DemoWindow.h"
#include "RmlUi_Backend_GLFW_GL3.h"
#include "RmlUi_Backend_GLFW_GL3_Data.h"
#include "Shell.h"

int main(int argc, char** argv) {
  if (!Shell::Initialize()) {
    spdlog::error("Unable to initialize Shell");
    return -1;
  }

  const char* kWindowTitle = "Hello RmlUi";
  constexpr int kWindowWidth = 1280;
  constexpr int kWindowHeight = 800;
  constexpr bool kAllowResize = true;

  Rml::SharedPtr<RmlUi::Backend::glfwGL3> backend =
      Rml::MakeShared<RmlUi::Backend::glfwGL3>();

  if (!backend->Initialize(kWindowTitle, kWindowWidth, kWindowHeight,
                           kAllowResize)) {
    spdlog::error("Unable to initialize RmlUi backend");
    return -1;
  }

  Rml::SetSystemInterface(backend->GetSystemInterface());
  Rml::SetRenderInterface(backend->GetRenderInterface());

  Rml::SharedPtr<RmlUi::Backend::DataGlfwGL3> data_backend =
      Rml::MakeShared<RmlUi::Backend::DataGlfwGL3>();

  Rml::Initialise();
  Rml::Debugger::Initialise(data_backend->context);

  auto demo_window =
      Rml::MakeShared<DemoWindow>("Demo sample", data_backend->context);

  DemoEventListenerInstancer event_listener_instancer;
  event_listener_instancer.SetBackend(backend);
  event_listener_instancer.SetWindow(demo_window);
  Rml::Factory::RegisterEventListenerInstancer(&event_listener_instancer);

  Shell::LoadFonts();

  demo_window->SetBackend(backend);
  demo_window->GetDocument()->AddEventListener(Rml::EventId::Keydown,
                                               demo_window.get());
  demo_window->GetDocument()->AddEventListener(Rml::EventId::Keyup,
                                               demo_window.get());
  demo_window->GetDocument()->AddEventListener(Rml::EventId::Animationend,
                                               demo_window.get());

  bool running = true;

  while (running) {
    running = backend->ProcessEvents(data_backend->context,
                                     &Shell::ProcessKeyDownShortcuts);

    demo_window->Update();
    data_backend->context->Update();

    backend->BeginFrame();
    data_backend->context->Render();
    backend->PresentFrame();
  }

  demo_window->Shutdown();
  Rml::Shutdown();
  backend->Shutdown();
  Shell::Shutdown();
  demo_window.reset();
}

//  // Setup window
// glfwSetErrorCallback(glfw_error_callback);
// if (!glfwInit()) return 1;
//
// const char* glsl_version = "#version 130";
// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//
//// Create window with graphics context
// GLFWwindow* window =
//     glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL,
//     NULL);
// if (window == NULL) return 1;
// glfwMakeContextCurrent(window);
// glfwSwapInterval(1);  // Enable vsync
//
//// Setup Dear ImGui context
// IMGUI_CHECKVERSION();
// ImGui::CreateContext();
// ImGuiIO& io = ImGui::GetIO();
//(void)io;
//// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
//// Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
//// Enable Gamepad Controls
//
//// Setup Dear ImGui style
// ImGui::StyleColorsDark();
//// ImGui::StyleColorsLight();
//
//// Setup Platform/Renderer backends
// ImGui_ImplGlfw_InitForOpenGL(window, true);
// ImGui_ImplOpenGL3_Init(glsl_version);
//
//// Load Fonts
//// - If no fonts are loaded, dear imgui will use the default font. You can
//// also load multiple fonts and use ImGui::PushFont()/PopFont() to select
//// them.
//// - AddFontFromFileTTF() will return the ImFont* so you can store it if you
//// need to select the font among multiple.
//// - If the file cannot be loaded, the function will return NULL. Please
//// handle those errors in your application (e.g. use an assertion, or display
//// an error and quit).
//// - The fonts will be rasterized at a given size (w/ oversampling) and stored
//// into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
//// ImGui_ImplXXXX_NewFrame below will call.
//// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype
//// for higher quality font rendering.
//// - Read 'docs/FONTS.md' for more instructions and details.
//// - Remember that in C/C++ if you want to include a backslash \ in a string
//// literal you need to write a double backslash \\ !
//// io.Fonts->AddFontDefault();
//// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
//// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
////
/// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//// ImFont* font =
//// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
//// NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);
//
//// Our state
// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
// Rml::String renderer_message;
//
// if (!RmlGL3::Initialize(&renderer_message)) {
//   spdlog::error("Unable to initialize RmlGL3");
//
//   // Cleanup
//   ImGui_ImplOpenGL3_Shutdown();
//   ImGui_ImplGlfw_Shutdown();
//   ImGui::DestroyContext();
//
//   glfwDestroyWindow(window);
//   glfwTerminate();
//
//   return -1;
// }
//
// data = Rml::MakeUnique<BackendData>();
//
// if (!data || !data->render_interface) {
//   spdlog::error("Failed to create backend data for rmlui");
//
//   // Cleanup
//   ImGui_ImplOpenGL3_Shutdown();
//   ImGui_ImplGlfw_Shutdown();
//   ImGui::DestroyContext();
//
//   glfwDestroyWindow(window);
//   glfwTerminate();
//
//   return -1;
// }
//
// data->window = window;
// data->system_interface.SetWindow(window);
// data->system_interface.LogMessage(Rml::Log::LT_INFO, renderer_message);
//
// int display_w, display_h;
// glfwGetFramebufferSize(window, &display_w, &display_h);
// data->render_interface.SetViewport(display_w, display_h);
//
// glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
// SetupCallbacks(window);
//
//// Main loop
// while (!glfwWindowShouldClose(window)) {
//   // Poll and handle events (inputs, window resize, etc.)
//   // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
//   // tell if dear imgui wants to use your inputs.
//   // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
//   // your main application, or clear/overwrite your copy of the mouse data.
//   // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
//   // data to your main application, or clear/overwrite your copy of the
//   // keyboard data. Generally you may always pass all inputs to dear imgui,
//   // and hide them from your application based on those two flags.
//   Backend::ProcessEvents(data->context, data->key_down_callback);
//
//   // Start the Dear ImGui frame
//   ImGui_ImplOpenGL3_NewFrame();
//   ImGui_ImplGlfw_NewFrame();
//   ImGui::NewFrame();
//
//   ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!"
//                                   // and append into it.
//
//   ImGui::Text("This is some useful text.");  // Display some text (you can
//                                              // use a format strings too)
//
//   ImGui::End();
//
//   // Rendering
//   ImGui::Render();
//
//   glfwGetFramebufferSize(window, &display_w, &display_h);
//   glViewport(0, 0, display_w, display_h);
//   data->render_interface.SetViewport(display_w, display_h);
//   glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
//                clear_color.z * clear_color.w, clear_color.w);
//   glClear(GL_COLOR_BUFFER_BIT);
//   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//   Backend::PresentFrame();
// }
//
//// Cleanup
// ImGui_ImplOpenGL3_Shutdown();
// ImGui_ImplGlfw_Shutdown();
// ImGui::DestroyContext();
//
// glfwDestroyWindow(window);
// data.reset();
// RmlGL3::Shutdown();
// glfwTerminate();
//
// return 0;