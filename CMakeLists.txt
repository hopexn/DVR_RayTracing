cmake_minimum_required(VERSION 3.7)
project(RayTracing)

set(CMAKE_CXX_STANDARD 11)

#OpenCV
find_package(OpenCV REQUIRED)

#QT5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)

#GL math
find_package(GLM REQUIRED)

#开启OpenGL
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
set(QT_USE_QTOPENGL TRUE)
add_definitions(${QT_DEFINITIONS})
include_directories(${QT_QTOPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

set(SOURCE_FILES main.cpp Volume.cpp Volume.h utils.h VolumeRender.cpp VolumeRender.h TF1D.cpp TF1D.h)

add_executable(RayTracing ${SOURCE_FILES})

target_link_libraries(RayTracing ${OpenCV_LIBS} Qt5::Widgets Qt5::OpenGL ${OPENGL_gl_LIBRARY})

