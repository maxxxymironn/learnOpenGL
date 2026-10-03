cmake_minimum_required(VERSION 3.20)
project(LearnOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(opengl)

target_compile_options(opengl PRIVATE -Wall -Wextra -Wpedantic)

target_sources(opengl PRIVATE
  src/main.cpp
  src/renderer.cpp
  src/window.cpp
  src/logger.cpp
)

target_compile_definitions(opengl PRIVATE
  SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/"
)

# glad
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad SYSTEM PUBLIC external/glad/include)

# glfw
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(external/glfw SYSTEM)

target_link_libraries(opengl PRIVATE
  glad
  glfw
)
