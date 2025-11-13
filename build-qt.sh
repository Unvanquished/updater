#!/usr/bin/env bash
set -e
set -u
set -o pipefail

prefix_output() {
    prefix="${1}"
    shift
    "$@" 2> >(sed "s/^/${prefix}/" >&2) | sed "s/^/${prefix}/"
}

module_vars() {
    url="https://download.qt.io/archive/qt/6.8/6.8.3/submodules/${1}-everywhere-src-6.8.3.tar.xz"
    archive="${url##*/}"
    dirname="${archive%.tar.*}"
}


build_module() {
    module="${1}"
    module_vars "${module}"
    cd "${WORK_DIR}/${dirname}"
    options_var="options_${module}"
    truncate_var="truncate_${module}"
    echo ${!truncate_var:-} | xargs --no-run-if-empty truncate -s 0
    if [[ "${module}" = qtbase ]]; then
        ./configure ${!options_var} ${common_options_cmake}
    else
        "${INSTALL_DIR}/bin/qt-configure-module" . ${!options_var} ${common_options_cmake}
    fi
    ninja
    cmake --install .
}

WORK_DIR="${PWD}"
SCRIPT_DIR=$(dirname "$0")
INSTALL_DIR="${WORK_DIR}/qt"

#FIXME bad warning in qtdeclarative-everywhere-src-6.8.3/src/quickwidgets/qquickwidget.cpp
# TODO: no-pie linux-only

common_options_cmake='
    -DFEATURE_accessibility=OFF
    -DQT_GENERATE_SBOM=OFF
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
    -qpa xcb
    -nomake tests
    -nomake examples
    -no-gif
    -no-icu
    -no-glib
    -opengl desktop
    -no-eglfs
    -no-opengles3
    -no-egl
    -xkbcommon
    -dbus-runtime
    -qt-freetype
    -qt-pcre
    -qt-harfbuzz
    -qt-libpng
    -qt-libjpeg
    -system-zlib
    -openssl-linked
    -prefix ${INSTALL_DIR}
    --
    -DFEATURE_androiddeployqt=OFF
    -DFEATURE_animation=ON
    -DFEATURE_cborstreamwriter=OFF
    -DFEATURE_columnview=OFF
    -DFEATURE_commandlinkbutton=OFF
    -DFEATURE_completer=OFF
    -DFEATURE_concatenatetablesproxy=OFF
    -DFEATURE_concurrent=OFF
    -DFEATURE_contextmenu=OFF
    -DFEATURE_cssparser=OFF
    -DFEATURE_datawidgetmapper=OFF
    -DFEATURE_datetimeedit=OFF
    -DFEATURE_dial=OFF
    -DFEATURE_dialogbuttonbox=ON
    -DFEATURE_dockwidget=OFF
    -DFEATURE_draganddrop=OFF
    -DFEATURE_dtls=OFF
    -DFEATURE_easingcurve=ON
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
    -DFEATURE_mdiarea=OFF
    -DFEATURE_menu=OFF
    -DFEATURE_menubar=OFF
    -DFEATURE_messagebox=ON
    -DFEATURE_mimetype=OFF
    -DFEATURE_mimetype_database=OFF
    -DFEATURE_movie=OFF
    -DFEATURE_networkdiskcache=OFF
    -DFEATURE_opengl_desktop=ON
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
    -DFEATURE_shortcut=OFF
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
    -DFEATURE_style_stylesheet=OFF
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
    -DFEATURE_xcb=ON
    -DFEATURE_xcb_glx_plugin=ON
    -DFEATURE_xml=OFF
    -DFEATURE_xmlstream=ON
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
    -DFEATURE_qml_locale=OFF
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
truncate_qtbase='src/tools/uic/CMakeLists.txt'

# qt5compat depends on qtdeclarative
# qtdeclarative depends on qtshadertools, qtsvg
MODULES=${@:-qtbase qtsvg qtshadertools qtdeclarative qt5compat}

# for bloaty
# options_qtbase="-force-debug-info ${options_qtbase}"

#FIXME
options_qtbase+=' -DFEATURE_dbus=OFF -DQT_FEATURE_dbus=OFF'

# Download in parallel
for module in $MODULES; do
    module_vars "${module}"
    if [[ ! -e "${archive}" ]]; then
        curl -LO "${url}" &
    fi
done
wait

md5sum --check --ignore-missing "${SCRIPT_DIR}/md5sums-qt.txt"

# Nuke old dir; extract in parallel
for module in $MODULES; do
    module_vars "${module}"
    rm -rf "${dirname}"
    tar -xJf "${archive}" &
done
wait

for module in $MODULES; do
    prefix_output "${module}| " build_module "${module}"
done
