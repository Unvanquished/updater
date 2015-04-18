cmake_minimum_required( VERSION 3.0 )
project(updater)

option(USE_STATIC "Build the updater using static Qt5" 0)
set(QT_DIR "" CACHE STRING "Directory to find Qt5 for static builds")

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(ZLIB REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIR})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

if (USE_STATIC)
  add_definitions(-DUSE_STATIC)
  set(QT5_LIBS "-L${QT_DIR}/lib -lQt5Widgets -lQt5Gui -lqtharfbuzzng -lQt5Network -lQt5Core")
  if (APPLE)
      set(OS_LIBS "-L${QT_DIR}/lib -lQt5PrintSupport -lcups -lQt5PlatformSupport -framework DiskArbitration -framework IOKit -framework ApplicationServices -framework CoreServices -framework CoreFoundation -framework Foundation -framework Cocoa -framework OpenGL -framework AGL -lQNetwork  -lQt5Core -lz -lm -framework Carbon -framework Cocoa -lz -framework OpenGL -framework AGL")
    set(PLATFORM_LIBS "-L${QT_DIR}/plugins/platforms -lqcocoa -framework Cocoa -framework Carbon")
  elseif (UNIX)
    # Qt5 hates CMake for static compiles
    set(OS_LIBS "-lpthread -lm -ldl -lrt")
    set(PLATFORM_LIBS "-L${QT_DIR}/plugins/platforms -lqxcb -lX11 -lX11-xcb -lXi -lSM -lICE -ldbus-1 -lxcb -L${QT_DIR}/lib -lxcb-static -lQt5PlatformSupport -lQt5DBus -lfontconfig -lfreetype -ludev")
  elseif (WIN32)
    set(PLATFORM_LIBS "-L${QT_DIR}/plugins/platforms -lqwindows -lwinspool -lshlwapi -liphlpapi -L${QT_DIR}/lib -lQt5PlatformSupport -lgdi32 -lcomdlg32 -loleaut32 -limm32 -lwinmm -lqtharfbuzzng -lQt5Core -lole32 -luuid -lws2_32 -ladvapi32 -lshell32 -luser32 -lkernel32 -lmpr")
  endif()
else() #STATIC
  set(QT5_LIBS Qt5::Widgets Qt5::Network Qt5::Gui)
endif() #USE_STATIC

if (APPLE)
    set(os_SOURCES osx.cpp)
elseif(UNIX)
    set(os_SOURCES unix.cpp)
elseif(WIN32)
    set(os_SOURCES win.cpp)
endif()

set(updater_SOURCES
  newsfetcher.cpp
  ariadownloader.cpp
  downloadworker.cpp
  main.cpp
  settingsdialog.cpp
  mainwindow.cpp
  ${os_SOURCES}
)

set(updater_HEADERS
  newsfetcher.h
  ariadownloader.h
  downloadworker.h
  settingsdialog.h
  mainwindow.h)

add_subdirectory(quazip)
set(updater_FORMS mainwindow.ui settingsdialog.ui)

QT5_WRAP_UI(updater_FORMS_HEADERS ${updater_FORMS})
QT5_ADD_RESOURCES(updater_RESOURCES_RCC resources.qrc)
add_executable(updater ${updater_SOURCES} ${updater_HEADERS} ${updater_FORMS_HEADERS} ${updater_RESOURCES_RCC} updater.rc)
set_property(TARGET updater PROPERTY CXX_STANDARD 11)
QT5_USE_MODULES(updater Widgets)
target_link_libraries(updater ${CMAKE_SOURCE_DIR}/aria2/src/.libs/libaria2.a quazip ${ZLIB_LIBRARIES} ${QT5_LIBS} ${PLATFORM_LIBS} ${OS_LIBS})
