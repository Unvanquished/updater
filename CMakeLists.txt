cmake_minimum_required(VERSION 3.18)

project(updater LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTORCC ON)

# Allow building with old CMake
set(QT_FORCE_MIN_CMAKE_VERSION_FOR_USING_QT 3.18)

# Silence warning with CMake < 3.19
set(QT_QML_GENERATE_AOTSTATS OFF)

set(ARIA2_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/aria2/src/.libs/libaria2.a" CACHE STRING "aria2 libraries")
set(ARIA2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/aria2/include" CACHE STRING "aria2 include directory")

find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick QuickControls2 QuickDialogs2   Network  Svg Gui Widgets          )

qt_standard_project_setup(REQUIRES 6.8)

option(FLUID_WITH_GALLERY "" OFF)
option(FLUID_WITH_DOCUMENTATION "" OFF)
option(FLUID_INSTALL_ICONS "" OFF)
add_subdirectory(fluid)

set(QUAZIP_BZIP2 OFF)
set(QUAZIP_ENABLE_TESTS OFF)
set(QUAZIP_FETCH_LIBS OFF)
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

if(APPLE)
    list(APPEND SOURCES osx.cpp)
elseif(WIN32)
    list(APPEND SOURCES win.cpp ExecInExplorer.cpp)
else()
    list(APPEND SOURCES linux.cpp)
endif()

qt_add_executable(updater MANUAL_FINALIZATION
    ${SOURCES}
    qml.qrc
)


# target_compile_definitions(updater PRIVATE QUAZIP_STATIC )

target_link_libraries(updater PRIVATE
    Qt6::Network
    Qt6::Widgets
)

target_link_libraries(updater INTERFACE
    Qt6::Qml
    Qt6::QuickDialogs2
    Qt6::Quick
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



target_link_libraries(updater PUBLIC
    QuaZip::QuaZip
    Fluid
)

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


qt_finalize_target(updater)

target_link_libraries(updater PUBLIC
    Qt6::Qml
    Qt6::Quick
)

