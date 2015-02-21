cmake_minimum_required( VERSION 3.1 )
project(updater)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(ZLIB REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

set(updater_SOURCES
  ariadownloader.cpp
  downloadworker.cpp
  main.cpp
  settingsdialog.cpp
  mainwindow.cpp)

set(updater_HEADERS
  ariadownloader.h
  downloadworker.h
  settingsdialog.h
  mainwindow.h)

add_subdirectory(quazip)
set(updater_FORMS mainwindow.ui settingsdialog.ui)

QT5_WRAP_UI(updater_FORMS_HEADERS ${updater_FORMS})
QT5_ADD_RESOURCES(updater_RESOURCES_RCC resources.qrc)
add_executable(updater ${updater_SOURCES} ${updater_HEADERS} ${updater_FORMS_HEADERS} ${updater_RESOURCES_RCC})
QT5_USE_MODULES(updater Widgets)
target_link_libraries(updater Qt5::Widgets ${CMAKE_SOURCE_DIR}/aria2/src/.libs/libaria2.a quazip z) 
set_property(TARGET updater PROPERTY CXX_STANDARD 11)
