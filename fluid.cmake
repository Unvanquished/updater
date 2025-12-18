# This file was created the same as fluid/src/controls/CMakeLists.txt in the
# fluid submodule commit, with a few changes:
# - rejigger QML module structure (rebased to upstream at https://github.com/lirios/fluid/pull/362)
# - make the library static
# - remove an install command
# - make it work regardless of CMAKE_CURRENT_SOURCE_DIR (i.e. without add_subdirectory)
# - remove the custom plugin class (controlsplugin.cpp) and use the auto-generated one. Thes
#   custom plugin class registers the image provider, which
#   we no longer use. Using a custom plugin class disables a ton of helpful Qt infrastructure for
#   automatic registration, and with the aggressive linker flags we are using, which don't follow
#   the C++ standard and remove things even from referenced translation units, it's very difficult
#   to get it to stop removing the plugin registration code.

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fluid/src/controls")

set(LIBNAME Fluid)

# Source files
set(_sources
    # cpp/controlsplugin.cpp cpp/controlsplugin.h
    cpp/core/clipboard.cpp cpp/core/clipboard.h
    cpp/core/controlsutils.cpp cpp/core/controlsutils.h
    cpp/core/device.cpp cpp/core/device.h
    cpp/core/inputregion.cpp cpp/core/inputregion.h
    cpp/core/standardpaths.cpp cpp/core/standardpaths.h
    cpp/datetime/datepicker.cpp cpp/datetime/datepicker.h
    cpp/datetime/dateselector.cpp cpp/datetime/dateselector.h
    cpp/datetime/datetimepicker.cpp cpp/datetime/datetimepicker.h
    cpp/datetime/dateutils.cpp cpp/datetime/dateutils.h
    cpp/datetime/picker.cpp cpp/datetime/picker.h
    cpp/datetime/timepicker.cpp cpp/datetime/timepicker.h
    cpp/datetime/timeselector.cpp cpp/datetime/timeselector.h
    cpp/datetime/yearmodel.cpp cpp/datetime/yearmodel.h
    cpp/datetime/yearselector.cpp cpp/datetime/yearselector.h
    cpp/iconthemeimageprovider.cpp cpp/iconthemeimageprovider.h
    cpp/style/color.cpp cpp/style/color.h
    # cpp/style/style.cpp cpp/style/style.h # breaks Mac build
)

# QML files
file(GLOB_RECURSE _qml CONFIGURE_DEPENDS
    RELATIVE "${SOURCE_DIR}"
    "${SOURCE_DIR}/qml/*.qml" "${SOURCE_DIR}/qml/**/*.qml"
)
foreach(_qmlfile ${_qml})
    set_source_files_properties("${SOURCE_DIR}/${_qmlfile}" PROPERTIES QT_RESOURCE_ALIAS "${_qmlfile}")
endforeach()

string(REGEX REPLACE cpp/ "${SOURCE_DIR}/cpp/" _sources "${_sources}")
string(REGEX REPLACE qml/ "${SOURCE_DIR}/qml/" _qml "${_qml}")

# ------------------------------------------------------------
# Define the QML module
# ------------------------------------------------------------
set_source_files_properties("${SOURCE_DIR}/qml/core/Units.qml" PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${LIBNAME} STATIC
    URI Fluid
    VERSION 2.0
    SOURCES ${_sources}
    QML_FILES ${_qml}
    IMPORTS
        QtQuick.Layouts
    DEPENDENCIES
        QtQuick
        QtQuick.Templates
        #Fluid.Private
    PLUGIN_TARGET ${LIBNAME}
    CLASS_NAME FluidPlugin
    NO_PLUGIN_OPTIONAL
)

set_target_properties(${LIBNAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
target_include_directories(${LIBNAME}
    PUBLIC
        ${SOURCE_DIR}/cpp/core
        ${SOURCE_DIR}/cpp/datetime
        ${SOURCE_DIR}/cpp/style
)
target_link_libraries(${LIBNAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Svg
)
target_compile_definitions(${LIBNAME}
    PRIVATE
        FLUID_INSTALL_ICONS=$<IF:$<BOOL:${FLUID_INSTALL_ICONS}>,1,0>
)

install(TARGETS ${LIBNAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# qt_install_qml_module(${LIBNAME})

# grep iconUrl *.qml | grep -oP "(?<=['\"]).*(?=['\"])" | sort -u
# TODO automate?
set(_icons
    "${SOURCE_DIR}/icons/action/settings.svg"
    "${SOURCE_DIR}/icons/av/pause.svg"
    "${SOURCE_DIR}/icons/av/play_arrow.svg"
    "${SOURCE_DIR}/icons/file/file_download.svg"
    "${SOURCE_DIR}/icons/file/folder.svg"
    "${SOURCE_DIR}/icons/navigation/cancel.svg"
    "${SOURCE_DIR}/icons/navigation/chevron_left.svg"
    "${SOURCE_DIR}/icons/navigation/chevron_right.svg"
)

qt_add_resources(${LIBNAME} FluidIcons
    PREFIX "/liri.io/fluid"
    BASE "${SOURCE_DIR}"
    FILES ${_icons}
)

