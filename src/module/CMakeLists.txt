cmake_minimum_required(VERSION 3.14)

project(module LANGUAGES CXX)

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
  alarmstateall.cpp
  alarmstateall.h
  basealarm.cpp
  basealarm.h
  journals.cpp
  journals.h
  journkdv.cpp
  journkdv.h
  journkiv.cpp
  journkiv.h
  journktf.cpp
  journktf.h
  modulealarm.cpp
  modulealarm.h
  module.cpp
  module.h
  ../../settings/settings.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE QXlsx)

#add_dependencies(${PROJECT_NAME} QXlsxBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE INTERFACES_LIBRARY)
