cmake_minimum_required(VERSION 3.8)

include(${CMAKE_SOURCE_DIR}/.glamour/glamour.cmake)
include(${GLAMOUR_PATH}/solution_preamble.cmake)

### Move to glamour
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

# Configure projects
include(${GLAMOUR_PATH}/solution_postamble.cmake)

### Move to glamour
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Configure Conan for now
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
  REQUIRES fmt/9.1.0
  REQUIRES glad/0.1.36
  REQUIRES glfw/3.3.8
  REQUIRES imgui/cci.20220621+1.88.docking
  REQUIRES spdlog/1.11.0

  GENERATORS cmake_multi
)

foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
    conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})
    conan_cmake_install(PATH_OR_REFERENCE .
                        BUILD missing
                        REMOTE conancenter
                        SETTINGS ${settings})
endforeach()

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

# Add  projects
add_subdirectory(projects)

# Finalize solution
include(${GLAMOUR_PATH}/solution_conclusion.cmake)
