cmake_minimum_required(VERSION 3.14)

project(startup LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  abstractstartupdialog.cpp
  abstractstartupdialog.h
  startupkdvdialog.cpp
  startupkdvdialog.h
  startupkivdialog.cpp
  startupkivdialog.h
  startupktfdialog.cpp
  startupktfdialog.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_compile_definitions(${PROJECT_NAME} PRIVATE INTERFACES_LIBRARY)
