#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

GLFWwindow* NewWindow(const int width, const int height, const char* title) {
  if (!glfwInit()) {
    spdlog::error("Unable to initialize GLFW");
    exit(-1);
  }

  return glfwCreateWindow(width, height, title, NULL, NULL);
}

void Shutdown() { glfwTerminate(); }

auto main() -> int {
  constexpr int kWindowWidth = 640;
  constexpr int kWindowHeight = 480;
  const char* kWindowTitle = "Hello world!";

  GLFWwindow* window = NewWindow(kWindowWidth, kWindowHeight, kWindowTitle);

  if (nullptr == window) {
    spdlog::error("Unable to create window");
    Shutdown();
    return -1;
  }

  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  Shutdown();
}