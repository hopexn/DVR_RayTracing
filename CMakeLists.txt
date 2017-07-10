cmake_minimum_required(VERSION 3.7)
project(RayTracing)

set(CMAKE_CXX_STANDARD 11)

#OpenCV
find_package(OpenCV REQUIRED)

#QT5
find_package(Qt5Widgets REQUIRED)

#GL math
find_package(GLM REQUIRED)

#开启OpenGL
find_package(OpenGL REQUIRED)
set(QT_USE_QTOPENGL TRUE)
add_definitions(${QT_DEFINITIONS})
include_directories(${QT_QTOPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

set(SOURCE_FILES main.cpp Volume.cpp Volume.h utils.h)

add_executable(RayTracing ${SOURCE_FILES})

target_link_libraries(RayTracing ${OpenCV_LIBS})