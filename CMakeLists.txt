cmake_minimum_required(VERSION 3.21)

project(updater LANGUAGES CXX)

# On Mac, default to x86_64 unless user requests something else
if (NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()

# Incompatible with static build among other issues
option(USE_ORIGINAL_FLUID_CMAKE "Use Fluid's own build system instead of fluid.cmake" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# triggers in QML generated files
if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
endif()

# important for quazip
set(BUILD_SHARED_LIBS OFF)

set(CMAKE_AUTORCC ON)

# Aria2 depends on zlib and possibly OpenSSL but those will be brought in
# by Qt anyway in the default setup.
set(ARIA2_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/aria2/src/.libs/libaria2.a" CACHE STRING "aria2 libraries")
set(ARIA2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/aria2/include" CACHE STRING "aria2 include directory")

find_package(Qt6 6.8
    REQUIRED
    COMPONENTS
        Core
        Network
        Qml
        Widgets
)

if (WIN32)
    set(CMAKE_C_FLAGS "-static -static-libgcc ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-static -static-libgcc ${CMAKE_CXX_FLAGS}")
endif()

qt_standard_project_setup(REQUIRES 6.8)

option(FLUID_INSTALL_ICONS "" OFF)
option(FLUID_WITH_DOCUMENTATION "" OFF)
option(FLUID_WITH_GALLERY "" OFF)
option(FLUID_WITH_QML_MODULES "" ON)
function(add_fluid) # Use function to emulate the new scope created by add_subdirectory
    find_package(Qt6 6.8
        REQUIRED
        COMPONENTS
            Core
            Core5Compat
            Gui
            Svg
            Qml
            Quick
            QuickControls2
    )
    if (NOT TARGET Qt6::GuiPrivate) # Arch Linux workaround
        find_package(Qt6 6.8 REQUIRED COMPONENTS GuiPrivate)
    endif()
    set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml_modules")
    include(fluid.cmake)
endfunction()

if (USE_ORIGINAL_FLUID_CMAKE)
    add_subdirectory(fluid)
else()
    add_fluid()
endif()

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
    list(APPEND SOURCES win.cpp ExecInExplorer.cpp updater.rc)
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
    Qt6::Core
    Qt6::Network
    Qt6::Qml
    Qt6::Widgets
    QuaZip::QuaZip
    Fluid
    ${ARIA2_LIBS}
)

# Turn on warning errors since the warnings are hard to see in Docker
if (NOT APPLE)
    target_compile_options(updater PRIVATE -Werror)
endif()

# macOS app bundle icon
if (APPLE)
    target_sources(updater PRIVATE resources/Unvanquished.icns)
    set_source_files_properties(resources/Unvanquished.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(updater PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE Unvanquished.icns
    )
endif()
