cmake_minimum_required(VERSION 3.21)

project(updater LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# triggers in QML generated files
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")

# important for quazip
set(BUILD_SHARED_LIBS OFF)

set(CMAKE_AUTORCC ON)

set(ARIA2_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/aria2/src/.libs/libaria2.a" CACHE STRING "aria2 libraries")
set(ARIA2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/aria2/include" CACHE STRING "aria2 include directory")

find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick QuickControls2 QuickDialogs2   Network  Svg Gui Widgets          )

# Actually only used by Fluid and Quazip, not us. HACK: prevent warning spam
find_package(Qt6 COMPONENTS Core5Compat)

if (WIN32)
    set(CMAKE_C_FLAGS "-static -static-libgcc ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-static -static-libgcc ${CMAKE_CXX_FLAGS}")
endif()

qt_standard_project_setup(REQUIRES 6.8)

option(FLUID_WITH_GALLERY "" OFF)
option(FLUID_WITH_DOCUMENTATION "" OFF)
option(FLUID_INSTALL_ICONS "" OFF)
add_subdirectory(fluid)

set(QUAZIP_BZIP2 OFF)
set(QUAZIP_ENABLE_TESTS OFF)
set(QUAZIP_FETCH_LIBS OFF)
if (WIN32)
   set(QUAZIP_USE_QT_ZLIB ON)
endif()
add_subdirectory(quazip)

set(SOURCES
    ariadownloader.cpp ariadownloader.h
    currentversionfetcher.cpp currentversionfetcher.h
    downloadtimecalculator.cpp downloadtimecalculator.h
    downloadworker.cpp downloadworker.h
    gamelauncher.cpp gamelauncher.h
    main.cpp
    qmldownloader.cpp qmldownloader.h
    settings.cpp settings.h
    splashcontroller.cpp splashcontroller.h
    system.h
)
set(QML_SOURCES
    DownloadInfo.qml
    main.qml
    News.qml
    NewsCard.qml
    Settings.qml
    splash.qml
    UpdateFailed.qml
    utils.js
)

if(APPLE)
    list(APPEND SOURCES osx.cpp)
elseif(WIN32)
    list(APPEND SOURCES win.cpp ExecInExplorer.cpp)
else()
    list(APPEND SOURCES linux.cpp)
endif()

qt_add_executable(updater WIN32
    ${SOURCES}
    qml.qrc
)

qt_add_qml_module(updater
    URI UnvUpdater
    QML_FILES ${QML_SOURCES}
    RESOURCE_PREFIX "/"
)

target_link_libraries(updater PRIVATE
    Qt6::Network
    Qt6::Widgets
    QuaZip::QuaZip
    Fluid
    ${ARIA2_LIBS}
)

target_link_libraries(updater PRIVATE
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickDialogs2
)


# Platform-specific linking
if(WIN32)
    target_link_libraries(updater PRIVATE
        crypt32
        secur32
        ole32
    )
    #
    target_sources(updater PRIVATE updater.rc)
elseif(UNIX)
    target_link_libraries(updater PRIVATE
        z
    )
endif()

# Turn on warning errors since the warnings are hard to see in Docker
if(NOT APPLE)
    target_compile_options(updater PRIVATE -Werror)
endif()

# macOS app bundle icon
# UNTESTED, AISLOP
if(APPLE)
    set_target_properties(updater PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE Unvanquished.icns
    )
    set(MACOSX_BUNDLE_ICON_FILE resources/Unvanquished.icns)
    set_source_files_properties(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )
    target_sources(updater PRIVATE ${MACOSX_BUNDLE_ICON_FILE})
endif()


