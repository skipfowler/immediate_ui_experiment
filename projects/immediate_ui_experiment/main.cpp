#include <fmt/core.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <entt/entt.hpp>

#include "Components/application_error.h"
#include "Entities/application.h"
#include "Tags/application_tag.h"
#include "git_wrapper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

static void glfw_error_callback(int error, const char* description) {
  spdlog::error("Glfw Error {}: {}", error, description);
}

auto main(int argc, char** argv) -> int {
  auto async_file_logger = spdlog::basic_logger_mt<spdlog::async_factory>(
      "async_file_logger", "logs/log.txt", true);
  spdlog::set_default_logger(async_file_logger);

  auto registry = std::make_shared<entt::registry>();
  Entities::Application application(registry);

  Wrapper::Git git_wrapper;

  git_wrapper.ErrorCallback([&](const int error_code, const char* description) {
    auto view = registry->view<Tags::ApplicationTag>();

    for (auto [entity] : view.each()) {
      auto& error = registry->emplace<Components::ApplicationError>(entity);
      error.description = description;
      error.error_code = error_code;
    }
  });

  auto repo_root_path = git_wrapper.Root(".");
  spdlog::info("Repository root path: {}", repo_root_path);

  const char* kWindowTitle = "burst";
  constexpr int kWidthStart = 1280;
  constexpr int kHeightStart = 780;

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(
      1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
  if (window == NULL) return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad
  // Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Application Errors");

    auto view = registry->view<Components::ApplicationError>();

    for (auto [entity, app_error] : view.each()) {
      ImGui::Text("git error %i: %s", app_error.error_code,
                  app_error.description.c_str());
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

// entt::registry registry;

// Create application management
// const auto application_management = registry.create();

// Read HEAD on master
// char head_filepath[512];
// FILE* head_fileptr;
// char head_rev[41];

// strcpy(head_filepath, root.ptr);

// if (strrchr(".git", '/') != (".git" + strlen(".git")))
//   strcat(head_filepath, "/refs/heads/main");
// else
//   strcat(head_filepath, "refs/heads/main");

// if ((head_fileptr = fopen(head_filepath, "r")) == NULL) {
//   fprintf(stderr, "Error opening '%s'\n", head_filepath);
//   return 1;
// }

// if (fread(head_rev, 40, 1, head_fileptr) != 1) {
//   fprintf(stderr, "Error reading from '%s'\n", head_filepath);
//   fclose(head_fileptr);
//   return 1;
// }

// git_oid oid;
// git_revwalk* walker;
// git_commit* commit;

// if (git_oid_fromstr(&oid, head_rev) != GIT_OK) {
//   fprintf(stderr, "Invalid git object: '%s'\n", head_rev);
//   return 1;
// }

// git_revwalk_new(&walker, repo);
// git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
// git_revwalk_push(walker, &oid);

// const char* commit_message;
// const git_signature* commit_author;

// while (git_revwalk_next(&oid, walker) == GIT_OK) {
//   if (git_commit_lookup(&commit, repo, &oid)) {
//     fprintf(stderr, "Failed to lookup the next object\n");
//     return 1;
//   }
//   git_buf signature = {0};
//   auto commit_id = const_cast<git_oid*>(git_commit_id(commit));
//   auto commit_id = git_commit_extract_signature(&signature,  commit,
// repo,
//   commit_id, nullptr); git_buf_free(&signature); commit_message =
//   git_commit_message(commit); commit_author =
// git_commit_committer(commit);

//  // Don't print the \n in the commit_message
//  printf("'%.*s' by %s <%s>\n", strlen(commit_message) - 1,
// commit_message,
//         commit_author->name, commit_author->email);

//  git_commit_free(commit);
//}

// git_revwalk_free(walker);

// git_repository_free(repo);
// git_buf_free(&root);

//// git_tree* tree = nullptr;
//// int error = git_commit_tree(&tree, commit);

// git_libgit2_shutdown();

// const char* kWindowTitle = "Immediate UI Experiment";
// const int kWidthStart = 1280;
// const int kHeightStart = 720;

//// Setup window
// glfwSetErrorCallback(glfw_error_callback);
// if (!glfwInit()) return 1;

//// GL 3.0 + GLSL 130
// const char* glsl_version = "#version 130";
// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+

//// Create window with graphics context
// GLFWwindow* window =
//     glfwCreateWindow(kWidthStart, kHeightStart, kWindowTitle, NULL,
// NULL);
// if (window == NULL) return 1;
// glfwMakeContextCurrent(window);
// glfwSwapInterval(1);  // Enable vsync

//// Setup Dear ImGui context
// IMGUI_CHECKVERSION();
// ImGui::CreateContext();
// ImGuiIO& io = ImGui::GetIO();
//(void)io;
// io.ConfigFlags |=
//     ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
//// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
/// Gamepad / Controls
// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable
// Multi-Viewport
//                                                      // / Platform
// Windows
//// io.ConfigViewportsNoAutoMerge = true;
//// io.ConfigViewportsNoTaskBarIcon = true;

//// Setup Dear ImGui style
// ImGui::StyleColorsDark();
//// ImGui::StyleColorsLight();

//// When viewports are enabled we tweak WindowRounding/WindowBg so
/// platform / windows can look identical to regular ones.
// ImGuiStyle& style = ImGui::GetStyle();
// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
//   style.WindowRounding = 0.0f;
//   style.Colors[ImGuiCol_WindowBg].w = 1.0f;
// }

//// Setup Platform/Renderer backends
// ImGui_ImplGlfw_InitForOpenGL(window, true);
// ImGui_ImplOpenGL3_Init(glsl_version);

//// Load Fonts
//// - If no fonts are loaded, dear imgui will use the default
/// font. You can / also load multiple fonts and use
/// ImGui::PushFont()/PopFont() to select / them. / -
/// AddFontFromFileTTF() will return the ImFont* so you can store
/// it if
// you
//// need to select the font among multiple.
//// - If the file cannot be loaded, the function will
/// return NULL. Please / handle those errors in your
/// application (e.g. use an assertion, or
/// display / an error and quit). / - The fonts will be
/// rasterized at a
// given
/// size (w/ oversampling) and stored / into a
/// texture when calling
/// ImFontAtlas::Build()/GetTexDataAsXXXX(), which
/// / ImGui_ImplXXXX_NewFrame below will call. / -
/// Use '#define IMGUI_ENABLE_FREETYPE' in your
// imconfig
/// file to use Freetype / for higher
/// quality font rendering. / - Read
///'docs/FONTS.md' for more instructions
/// and details. / - Remember that in
/// C/C++ if you want to include a
/// backslash \ in a string / literal you
// need
/// to write a double backslash \\ ! / io.Fonts->AddFontDefault(); /
/// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);

/// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
/// /
///
// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
////
///
// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//// ImFont* font =
//// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
//// NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

//// Our state
// bool show_demo_window = true;
// bool show_another_window = false;
// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// constexpr ImGuiWindowFlags kTabRegionFlags =
//     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
//     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
//     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;

// constexpr ImGuiWindowFlags kMainWindowFlags =
//     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
//     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
//     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar |
//     ImGuiWindowFlags_NoDocking;

//// Main loop
// while (!glfwWindowShouldClose(window)) {
//   // Poll and handle events (inputs, window resize, etc.)
//   // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags
// to
//   // tell if dear imgui wants to use your inputs.
//   // - When io.WantCaptureMouse is true, do not dispatch mouse input data
//   to
//   // your main application, or clear/overwrite your copy of the mouse
// data.
//   // - When io.WantCaptureKeyboard is true, do not dispatch keyboard
// input
//   // data to your main application, or clear/overwrite your copy of the
//   // keyboard data. Generally you may always pass all inputs to dear
// imgui,
//   // and hide them from your application based on those two flags.
//   glfwPollEvents();

//  int display_w, display_h;
//  glfwGetFramebufferSize(window, &display_w, &display_h);

//  // Start the Dear ImGui frame
//  ImGui_ImplOpenGL3_NewFrame();
//  ImGui_ImplGlfw_NewFrame();
//  ImGui::NewFrame();

//  const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
//  ImGui::SetNextWindowPos(
//      ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y),
//      ImGuiCond_Always);
//  ImGui::SetNextWindowSize(
//      ImVec2(static_cast<float>(display_w),
// static_cast<float>(display_h)));

//  ImGui::Begin(kWindowTitle, nullptr, kMainWindowFlags);

//  if (ImGui::BeginMenuBar()) {
//    if (ImGui::BeginMenu("File")) {
//      if (ImGui::MenuItem("Close", "Ctrl+W")) {
//        glfwSetWindowShouldClose(window, true);
//      }
//      ImGui::EndMenu();
//    }
//    ImGui::EndMenuBar();
//  }

//  ImGui::End();

//  constexpr int kMainMenuHeight = 20;

//  ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x,
//                                 main_viewport->WorkPos.y +
//                                 kMainMenuHeight),
//                          ImGuiCond_Always);
//  ImGui::SetNextWindowSize(
//      ImVec2(static_cast<float>(display_w),
//             static_cast<float>(display_h - kMainMenuHeight)));

//  ImGui::Begin("TabRegion", nullptr, kTabRegionFlags);
//  ImGui::End();

//  ImGui::Begin("Branches");

//  for (auto& branch_name : branch_names) {
//    ImGui::Text(branch_name.c_str());
//  }
//
//  ImGui::End();

//  ImGui::Begin("Commits");
//  ImGui::Text("Hello from subordiate 2!");
//  ImGui::End();

//  // Rendering
//  ImGui::Render();

//  glViewport(0, 0, display_w, display_h);
//  glClearColor(clear_color.x * clear_color.w, clear_color.y *
// clear_color.w,
//               clear_color.z * clear_color.w, clear_color.w);
//  glClear(GL_COLOR_BUFFER_BIT);
//  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//  // Update and Render additional Platform Windows
//  // (Platform functions may change the current OpenGL context, so we
//  // save/restore it to make it easier to paste this code elsewhere.
//  //  For this specific demo app we could also call
//  //  glfwMakeContextCurrent(window) directly)
//  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
//    GLFWwindow* backup_current_context = glfwGetCurrentContext();
//    ImGui::UpdatePlatformWindows();
//    ImGui::RenderPlatformWindowsDefault();
//    glfwMakeContextCurrent(backup_current_context);
//  }

//  glfwSwapBuffers(window);
//}

//// Cleanup
// ImGui_ImplOpenGL3_Shutdown();
// ImGui_ImplGlfw_Shutdown();
// ImGui::DestroyContext();

// glfwDestroyWindow(window);
// glfwTerminate();
//}