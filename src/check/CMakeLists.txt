cmake_minimum_required(VERSION 3.14)

project(check LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  abstractcheckdialog.cpp
  abstractcheckdialog.h
  check.cpp
  check.h
  checkkdvdialog.cpp
  checkkdvdialog.h
  checkkdvharmonicdialog.cpp
  checkkdvharmonicdialog.h
  checkkdvvibrdialog.cpp
  checkkdvvibrdialog.h
  checkkivdialog.cpp
  checkkivdialog.h
  checkktfdialog.cpp
  checkktfdialog.h
  checkktfharmonicdialog.cpp
  checkktfharmonicdialog.h
  check3533dialog.h
  check3533dialog.cpp)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
