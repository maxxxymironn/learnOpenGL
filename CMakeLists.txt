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

  src/core/logger.cpp
  src/core/timeHandler.cpp

  src/render/renderer.cpp
  src/render/shader.cpp
  src/render/model/mesh.cpp
  src/render/model/model.cpp

  src/window/window.cpp
)

target_compile_definitions(opengl PRIVATE
  SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/"
  IMAGES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/"
  MODELS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/models/"
)

# glad
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad SYSTEM PUBLIC external/glad/include)

# glfw
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(external/glfw SYSTEM)

# asssimp
add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/external/assimp/lib/libassimp.so.6"
)

target_include_directories(opengl PRIVATE
  ${CMAKE_SOURCE_DIR}/external
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp
)

target_link_libraries(opengl PRIVATE
  glad
  glfw
  assimp
)
