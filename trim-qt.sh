#! /usr/bin/env bash

cd "${1:-.}"

# Replace unused embedded files with empty ones to save space.
for f in \
	qtbase-everywhere-src-*/src/widgets/dialogs/images/*.png \
	qtbase-everywhere-src-*/src/widgets/styles/images/*.png \
	qtdeclarative-everywhere-src-*/src/quickcontrols/basic/images/*.png \
	qtdeclarative-everywhere-src-*/src/quickcontrols/material/images/*.png \
	qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/images/imagine/*.png
do
	truncate -s0 "${f}"
done

for f in qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/images/*.png
do
	case "${f}" in
		# Used in the installation folder pickup window.
		*/crumb-separator-icon-square.png);;
		*/folder-icon-square.png);;
		*/up-icon-thick-square.png);;
		*) truncate -s0 "${f}";;
	esac
done

for f in qtdeclarative-everywhere-src-*/src/quickcontrols/basic/*.qml
do
	echo 'T{}' > "${f}"
done

for f in qtdeclarative-everywhere-src-*/src/quickcontrols/material/*.qml
do
	case "${f}" in
		# Required by the updater itself.
		*/ApplicationWindow.qml);;
		*/BusyIndicator.qml);;
		*/Button.qml);;
		*/Drawer.qml);;
		*/ItemDelegate.qml);;
		*/Label.qml);;
		*/PageIndicator.qml);;
		*/Pane.qml);;
		*/Popup.qml);;
		*/ProgressBar.qml);;
		*/RoundButton.qml);;
		*/SwipeView.qml);;
		*/TextField.qml);;
		# Required by FolderDialog.qml.
		*/DialogButtonBox.qml);;
		*/ScrollBar.qml);;
		*/ToolButton.qml);;
		*) echo 'T{}' > "${f}";;
	esac
done

for f in \
	qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/qml/+Fusion/*.qml \
	qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/qml/+Imagine/*.qml \
	qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/qml/+Universal/*.qml
do
	echo 'T{}' > "${f}"
done

for f in qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/qml/+Material/*.qml
do
	case "${f}" in
		*/FolderDialog.qml);;
		# Required by FolderDialog.qml.
		*/FolderBreadcrumbBar.qml);;
		*/FolderDialogDelegate.qml);;
		*) echo 'T{}' > "${f}";;
	esac
done

for f in qtdeclarative-everywhere-src-*/src/quickdialogs/quickdialogsquickimpl/qml/*.qml
do
	case "${f}" in
		# Required by FolderDialog.qml.
		*/FolderDialogDelegateLabel.qml);;
		*) echo 'T{}' > "${f}";;
	esac
done
