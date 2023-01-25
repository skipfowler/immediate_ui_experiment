project(
  ${PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME}
  VERSION ${SOLUTION_VERSION}
  LANGUAGES CXX
)

add_executable(
  ${PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME}
  "main.cpp"
  "DemoEventListener.cpp"
  "DemoEventListener.h"
  "DemoEventListenerInstancer.cpp"
  "DemoEventListenerInstancer.h"
  "DemoWindow.cpp"
  "DemoWindow.h"
  "PlatformExtensions.cpp"
  "PlatformExtensions.h"
  "RendererExtensions.cpp"
  "RendererExtensions.h"
  "Shell.cpp"
  "Shell.h"
  "ShellFileInterface.cpp"
  "ShellFileInterface.h"
)

target_include_directories(
  ${PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME} PRIVATE
  ${PROJECTS_IMGUI_BINDINGS_INCLUDE_DIRECTORIES}
  ${PROJECTS_RMLUI_BACKENDS_INCLUDE_DIRECTORIES}
)

target_link_libraries(
  ${PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME}
  ${PROJECTS_IMGUI_BINDINGS_TARGET_NAME}
  ${PROJECTS_RMLUI_BACKENDS_TARGET_NAME}
  CONAN_PKG::fmt
  CONAN_PKG::glad
  CONAN_PKG::glfw
  CONAN_PKG::imgui
  CONAN_PKG::rmlui
  CONAN_PKG::spdlog
)

if(WIN32)
  target_link_libraries(
    ${PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME}
    shlwapi.lib
  )
endif()