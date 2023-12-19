cmake_minimum_required(VERSION 2.8)
set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_CXX_STANDARD 14)
find_package(glm REQUIRED)

PROJECT(NEAVC)

configure_file("./test/fragmentShader.gs" "${CMAKE_BINARY_DIR}/fragmentShader.gs" COPYONLY)
configure_file("./test/vertexShader.gs" "${CMAKE_BINARY_DIR}/vertexShader.gs" COPYONLY)

add_subdirectory("./src")

enable_testing()
add_definitions(-D__DEBUG__)
add_subdirectory("./test")
