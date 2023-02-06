cmake_minimum_required(VERSION 3.8)

### Move to glamour
set(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(SOLUTION_NAME "immediate_ui_experiment")
set(SOLUTION_VERSION "1.0.0")

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

# Create solution
project(
  ${SOLUTION_NAME}
  VERSION ${SOLUTION_VERSION}
  LANGUAGES CXX
)

### Move to glamour
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Subsystem linker options
if (WIN32)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
else()
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# Configure Conan for now
set(GENERATED_CONAN_CMAKE_PATH ${CMAKE_BINARY_DIR}/generated/conan/cmake)
list(APPEND CMAKE_MODULE_PATH ${GENERATED_CONAN_CMAKE_PATH})
list(APPEND CMAKE_PREFIX_PATH ${GENERATED_CONAN_CMAKE_PATH})

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
  REQUIRES entt/3.11.1
  REQUIRES fmt/9.1.0
  REQUIRES glad/0.1.36
  REQUIRES glfw/3.3.8
  REQUIRES imgui/cci.20230123+1.89.2.docking
  REQUIRES libgit2/1.5.0
  REQUIRES nanobench/4.3.9
  REQUIRES rapidjson/cci.20220822
  REQUIRES spdlog/1.11.0

  GENERATORS CMakeDeps
)

foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
    conan_cmake_autodetect(
      settings
      BUILD_TYPE ${TYPE}
    )

    conan_cmake_install(
      PATH_OR_REFERENCE .
      BUILD missing
      REMOTE conancenter
      SETTINGS ${settings}
      INSTALL_FOLDER ${GENERATED_CONAN_CMAKE_PATH}
      OUTPUT_FOLDER ${GENERATED_CONAN_CMAKE_PATH}
    )
endforeach()

find_package(entt REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
find_package(glad REQUIRED CONFIG)
find_package(glfw3 REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(libgit2 REQUIRED CONFIG)
find_package(nanobench REQUIRED CONFIG)
find_package(rapidjson REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)

set(PROJECTS_IMGUI_BINDINGS_TARGET_NAME "imgui_bindings")
set(PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME "immediate_ui_experiment")
set(PROJECTS_AZURE_DEVOPS_EXPERIMENT_TARGET_NAME "azure_devops_experiment")
set(PROJECTS_IMMEDIATE_UI_EXPERIMENT_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/projects/immediate_ui_experiment")
set(PROJECTS_IMGUI_BINDINGS_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/projects/imgui_bindings")
set(PROJECTS_AZURE_DEVOPS_EXPERIMENT_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/projects/azure_devops_experiment")

# Add  projects
add_subdirectory(projects)

### Move to glamour
set_property(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
  PROPERTY VS_STARTUP_PROJECT 
  ${PROJECTS_IMMEDIATE_UI_EXPERIMENT_TARGET_NAME}
)