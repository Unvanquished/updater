#! /usr/bin/env bash

set -u -e -o pipefail

cd "${1:-.}"

used_svg="
icons/action/settings.svg
icons/av/pause.svg
icons/av/play_arrow.svg
icons/file/file_download.svg
icons/file/folder.svg
icons/navigation/cancel.svg
icons/navigation/chevron_left.svg
icons/navigation/chevron_right.svg
"

used_qml="
# Directly accessed.
qml/Card.qml
qml/SnackBar.qml
qml/ThinDivider.qml
qml/buttons/FloatingActionButton.qml
qml/controls/Subheader.qml
qml/core/Units.qml
qml/labels/BodyLabel.qml
qml/labels/DisplayLabel.qml
qml/labels/TitleLabel.qml
qml/sheets/BottomSheet.qml
qml/sheets/BottomSheetGrid.qml
qml/sheets/BottomSheetList.qml

# Dependencies.
qml/effects/BoxShadow.qml
qml/effects/CircleMask.qml
qml/effects/Elevation.qml
qml/effects/Ripple.qml
"

printFilter() {
	printf '%s' "${1}" \
	| sed -e 's/#.*//;s/ *//g;s/\./\\./' \
	| grep -v '^$' \
	| tr '\n' '|' \
	| sed -e 's/|+/|/g;s/^|//;s/|$//'
}

find fluid/src/controls/icons -type f -name '*.svg' \
	| grep -Ev "$(printFilter "${used_svg}")" \
	| xargs -I{} -P1 rm -v {}

find fluid/src/controls/qml -type f -name '*.qml' \
	| grep -Ev "$(printFilter "${used_qml}")" \
	| xargs -I{} -P1 rm -v {}
