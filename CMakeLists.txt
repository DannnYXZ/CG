cmake_minimum_required(VERSION 3.13)
project(CG)

set(CMAKE_CXX_STANDARD 14)
add_executable(CG src/main.cpp deps/glad.c src/shader.h deps/stb_image/stb_image.h deps/stb_image/stb_image.cpp src/camera.h)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# configure GLFW
add_subdirectory(deps/glfw)
target_link_libraries(CG glfw)
find_package(OpenGL REQUIRED)
target_include_directories(CG PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(CG ${OPENGL_gl_LIBRARY})

# configure GLAD
include_directories(deps)