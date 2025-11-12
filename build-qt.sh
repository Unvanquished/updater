#!/usr/bin/env bash

# Usage: [BUILDQT_CMAKE_ARGS=<args...>] build-qt.sh <sysname> [<qt submodule>...]
# installs to ./qt_<sysname>/

set -e
set -u
set -o pipefail

prefix_output() {
    prefix="${1}"
    shift
    "$@" 2>&1 | sed "s/^/${prefix}/"
}

module_vars() {
    url="https://download.qt.io/archive/qt/6.8/6.8.3/submodules/${1}-everywhere-src-6.8.3.tar.xz"
    archive="${url##*/}"
    dirname="build_${sysname}/${archive%.tar.*}"
}


build_module() {
    module="${1}"
    module_vars "${module}"
    cd "${WORK_DIR}/${dirname}"
    options_var="options_${module}"
    find "${SCRIPT_DIR}/patches" -name "${module}-*.patch" -exec cat {} \; | patch -p1
    case "${module}" in
    qtbase)
        ./configure ${!options_var} ${common_options_cmake}
        ;;
    *)
        "${INSTALL_DIR}/bin/qt-configure-module" . ${!options_var} ${common_options_cmake}
        ;;
    esac
    ninja
    cmake --install .
}

sysname="${1}"
shift

WORK_DIR="${PWD}"
SCRIPT_DIR=$(realpath "$(dirname "$0")")
INSTALL_DIR="${WORK_DIR}/qt_${sysname}"

export CFLAGS="-Werror=return-type ${CFLAGS:-}"
export CXXFLAGS="-Werror=return-type ${CXXFLAGS:-}"

common_options_cmake="
    -DQT_GENERATE_SBOM=OFF
    ${BUILDQT_CMAKE_ARGS:-}
"

base_features='
    -DFEATURE_accessibility=OFF
    -DFEATURE_androiddeployqt=OFF
    -DFEATURE_animation=ON
    -DFEATURE_cborstreamwriter=OFF
    -DFEATURE_clipboard=ON
    -DFEATURE_columnview=OFF
    -DFEATURE_commandlinkbutton=OFF
    -DFEATURE_completer=OFF
    -DFEATURE_concatenatetablesproxy=OFF
    -DFEATURE_concurrent=OFF
    -DFEATURE_contextmenu=OFF
    -DFEATURE_cssparser=ON
    -DFEATURE_cursor=ON
    -DFEATURE_datawidgetmapper=OFF
    -DFEATURE_datetimeedit=OFF
    -DFEATURE_dbus=OFF
    -DFEATURE_dial=OFF
    -DFEATURE_dialogbuttonbox=ON
    -DFEATURE_dockwidget=OFF
    -DFEATURE_draganddrop=OFF
    -DFEATURE_dtls=OFF
    -DFEATURE_easingcurve=ON
    -DFEATURE_evdev=OFF
    -DFEATURE_filedialog=OFF
    -DFEATURE_filesystemmodel=OFF
    -DFEATURE_filesystemwatcher=OFF
    -DFEATURE_fontcombobox=OFF
    -DFEATURE_fontdialog=OFF
    -DFEATURE_formlayout=OFF
    -DFEATURE_fscompleter=OFF
    -DFEATURE_future=OFF
    -DFEATURE_gestures=OFF
    -DFEATURE_graphicsview=OFF
    -DFEATURE_groupbox=OFF
    -DFEATURE_hijricalendar=OFF
    -DFEATURE_identityproxymodel=OFF
    -DFEATURE_imageformat_bmp=OFF
    -DFEATURE_imageformat_jpeg=ON
    -DFEATURE_imageformat_png=ON
    -DFEATURE_imageformat_ppm=OFF
    -DFEATURE_imageformat_xbm=OFF
    -DFEATURE_imageformat_xpm=OFF
    -DFEATURE_imageformatplugin=ON
    -DFEATURE_imageio_text_loading=OFF
    -DFEATURE_inputdialog=OFF
    -DFEATURE_islamiccivilcalendar=OFF
    -DFEATURE_itemmodel=ON
    -DFEATURE_itemviews=OFF
    -DFEATURE_jalalicalendar=OFF
    -DFEATURE_keysequenceedit=OFF
    -DFEATURE_label=ON
    -DFEATURE_lcdnumber=OFF
    -DFEATURE_library=OFF
    -DFEATURE_lineedit=OFF
    -DFEATURE_listview=OFF
    -DFEATURE_listwidget=OFF
    -DFEATURE_localserver=OFF
    -DFEATURE_macdeployqt=OFF
    -DFEATURE_mdiarea=OFF
    -DFEATURE_menu=OFF
    -DFEATURE_menubar=OFF
    -DFEATURE_messagebox=ON
    -DFEATURE_mimetype=OFF
    -DFEATURE_mimetype_database=OFF
    -DFEATURE_movie=OFF
    -DFEATURE_networkdiskcache=OFF
    -DFEATURE_pdf=OFF
    -DFEATURE_picture=OFF
    -DFEATURE_printsupport=OFF
    -DFEATURE_progressbar=OFF
    -DFEATURE_proxymodel=OFF
    -DFEATURE_publicsuffix_qt=OFF
    -DFEATURE_publicsuffix_system=OFF
    -DFEATURE_pushbutton=ON
    -DFEATURE_qmake=OFF
    -DFEATURE_radiobutton=OFF
    -DFEATURE_rubberband=OFF
    -DFEATURE_scrollarea=OFF
    -DFEATURE_scrollbar=OFF
    -DFEATURE_scroller=OFF
    -DFEATURE_sessionmanager=OFF
    -DFEATURE_settings=ON
    -DFEATURE_shortcut=ON
    -DFEATURE_sizegrip=OFF
    -DFEATURE_slider=OFF
    -DFEATURE_socks5=OFF
    -DFEATURE_sortfilterproxymodel=OFF
    -DFEATURE_spinbox=OFF
    -DFEATURE_splitter=OFF
    -DFEATURE_sql=OFF
    -DFEATURE_stackedwidget=OFF
    -DFEATURE_standarditemmodel=OFF
    -DFEATURE_statusbar=OFF
    -DFEATURE_statustip=OFF
    -DFEATURE_stringlistmodel=OFF
    -DFEATURE_style_fusion=OFF
    -DFEATURE_style_mac=OFF
    -DFEATURE_style_stylesheet=OFF
    -DFEATURE_style_windows=ON
    -DFEATURE_style_windows11=OFF
    -DFEATURE_style_windowsvista=OFF
    -DFEATURE_syntaxhighlighter=OFF
    -DFEATURE_systemtrayicon=OFF
    -DFEATURE_tabbar=OFF
    -DFEATURE_tabletevent=OFF
    -DFEATURE_tableview=OFF
    -DFEATURE_tabwidget=OFF
    -DFEATURE_testlib=OFF
    -DFEATURE_textbrowser=OFF
    -DFEATURE_textedit=OFF
    -DFEATURE_textmarkdownreader=OFF
    -DFEATURE_textmarkdownwriter=OFF
    -DFEATURE_textodfwriter=OFF
    -DFEATURE_timezonelocale=ON
    -DFEATURE_toolbar=OFF
    -DFEATURE_toolbox=OFF
    -DFEATURE_toolbutton=OFF
    -DFEATURE_tooltip=OFF
    -DFEATURE_topleveldomain=OFF
    -DFEATURE_translation=OFF
    -DFEATURE_transposeproxymodel=OFF
    -DFEATURE_treeview=OFF
    -DFEATURE_treewidget=OFF
    -DFEATURE_tuiotouch=OFF
    -DFEATURE_udpsocket=OFF
    -DFEATURE_undocommand=OFF
    -DFEATURE_undogroup=OFF
    -DFEATURE_undostack=OFF
    -DFEATURE_undoview=OFF
    -DFEATURE_validator=OFF
    -DFEATURE_vkgen=OFF
    -DFEATURE_vnc=OFF
    -DFEATURE_whatsthis=OFF
    -DFEATURE_widgettextcontrol=ON
    -DFEATURE_wizard=OFF
    -DFEATURE_xml=OFF
    -DFEATURE_xmlstream=ON
'

options_qtbase="
    -opensource
    -confirm-license
    -release
    -optimize-size
    -no-shared
    -static
    --c++std=17
    -disable-deprecated-up-to 0x060800
    -reduce-exports
    -gc-binaries
    -nomake tests
    -nomake examples
    -no-gif
    -no-icu
    -no-glib
    -opengl desktop
    -no-eglfs
    -no-opengles3
    -no-egl
    -qt-freetype
    -qt-pcre
    -qt-harfbuzz
    -qt-libpng
    -qt-libjpeg
    -prefix ${INSTALL_DIR}
    --
    ${base_features}
    -DFEATURE_opengl_desktop=ON
    -DQT_BUILD_DOCS=OFF
"
options_qt5compat='--'
options_qtsvg='--'
options_qtshadertools='--'
options_qtdeclarative='
    --
    -DBUILD_WITH_PCH=OFF
    -DFEATURE_qml_animation=ON
    -DFEATURE_qml_debug=OFF
    -DFEATURE_qml_jit=OFF
    -DFEATURE_qml_locale=ON
    -DFEATURE_qml_profiler=OFF
    -DFEATURE_qml_xml_http_request=ON
    -DFEATURE_qml_xmllistmodel=OFF
    -DFEATURE_quick_designer=OFF
    -DFEATURE_quick_particles=OFF
    -DFEATURE_quick_tableview=OFF
    -DFEATURE_quick_treeview=OFF
    -DFEATURE_quickcontrols2_fluentwinui3=OFF
    -DFEATURE_quickcontrols2_fusion=OFF
    -DFEATURE_quickcontrols2_imagine=OFF
    -DFEATURE_quickcontrols2_universal=OFF
    -DFEATURE_quickcontrols2_windows=OFF
    -DFEATURE_quicktemplates2_calendar=OFF
    -DFEATURE_quicktemplates2_container=ON
    -DFEATURE_quicktemplates2_hover=OFF
    -DFEATURE_quicktemplates2_multitouch=OFF
'

case "${sysname}" in
linux)
    options_qtbase="
        -dbus-runtime
        -openssl-linked
        -qpa xcb
        -system-zlib
        -xkbcommon
        ${options_qtbase}
        -DFEATURE_xcb=ON
        -DFEATURE_xcb_glx_plugin=ON
    "
    ;;
macos)
    # I don't need to install Xcode
    common_options_cmake="-DQT_FORCE_WARN_APPLE_SDK_AND_XCODE_CHECK=ON ${common_options_cmake}"

    options_qtbase="${options_qtbase}
        -DFEATURE_draganddrop=ON -DFEATURE_imageformat_xpm=ON
        -DCMAKE_OSX_ARCHITECTURES=x86_64
        -DQT_NO_HANDLE_APPLE_SINGLE_ARCH_CROSS_COMPILING=1
        -DBUILD_WITH_PCH=OFF
    "
    ;;
windows)
    options_qtbase="
        -device-option CROSS_COMPILE=i686-w64-mingw32-
        -xplatform win32-g++
        -schannel
        ${options_qtbase}
        -DFEATURE_imageformat_xpm=ON
        -DFEATURE_library=ON
        -DCMAKE_TOOLCHAIN_FILE=${SCRIPT_DIR}/cross-toolchain-mingw32.cmake
        -DQT_HOST_PATH=${WORK_DIR}/qt_tools
    "
    ;;
tools)
    options_qtbase="
        --c++std=17
        -release
        -static
        -qpa none
        -no-opengl
        -no-harfbuzz
        -no-freetype
        -no-ico
        -no-gif
        -no-xkbcommon
        -no-openssl
        -prefix ${INSTALL_DIR}
        --
        ${base_features}
        -DFEATURE_widgets=OFF
        -DFEATURE_network=OFF
        -DFEATURE_harfbuzz=OFF
        -DFEATURE_evdev=OFF
        -DFEATURE_cursor=OFF
        -DFEATURE_linuxfb=OFF
        -DFEATURE_freetype=OFF
        -DFEATURE_libinput=OFF
        -DFEATURE_imageformat_jpeg=OFF
        -DFEATURE_imageformat_png=OFF
        -DFEATURE_imageformatplugin=OFF
    "
    options_qtdeclarative='
        --
    '
    ;;
*)
    echo 'arg 1 should be linux|macos|windows'
    exit 1
esac

# qt5compat depends on qtdeclarative
# qtdeclarative depends on qtshadertools
# qtdeclarative optionally depends on qtsvg but we don't actually want that?
# Also in the tools build we strategically build qtdeclarative before qtshadertools to disable
# the bulk of QML targets (TODO: disable them explicitly with options?)
MODULES=${@:-qtbase qtshadertools qtdeclarative qt5compat qtsvg}

# Download in parallel
for module in $MODULES; do
    module_vars "${module}"
    if [[ ! -e "${archive}" ]]; then
        curl -LO "${url}" &
    fi
done
wait

md5sum --check --ignore-missing "${SCRIPT_DIR}/md5sums-qt.txt"

mkdir -p "${WORK_DIR}/build_${sysname}"

# Nuke old dir; extract in parallel
for module in $MODULES; do
    module_vars "${module}"
    rm -rf "${dirname}"
    tar -C "${WORK_DIR}/build_${sysname}" -xJf "${archive}" &
done
wait

for module in $MODULES; do
    prefix_output "${module}| " build_module "${module}"
done
