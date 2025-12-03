#! /usr/bin/env bash

set -u -e

cd "$(dirname "${BASH_SOURCE[0]}")"

force=false
case "${1:-}" in
-f|--force) force=true;;
esac

touchFileFromGitDir()
{
	touch --date="$(git -C "${1}" --no-pager log -1 --format="%ai")" "${2}"
}

touchGitDir()
{
	touchFileFromGitDir "${1}" "${1}"
}

isNewer()
{
	touchGitDir "${1}"

	if "${force}"
	then
		return 0
	fi

	if [ ! -e "${2}" ]
	then
		return 0
	fi

	if [ "${1}" -nt "${2}" ]
	then
		return 0
	fi

	return 1
}

generateFont()
{
	local name="${1}"; shift
	local style="${1}"; shift
	local repo="${1}"; shift
	local codes="${1}"; shift

	local dir="fonts/${name}"
	local base="${name}-${style}"
	local package="${dir}/sources/${name}.glyphspackage"
	local space="${dir}/sources/${name}.designspace"

	[ -d "${dir}" ] || git clone "${repo}" "${dir}"

	git -C "${dir}" pull

	if isNewer "${dir}" "build/otf/${base}.otf"
	then
		git -C "${dir}" clean -fd

		[ -e "${space}" ] || glyphs2ufo "${package}"

		fontmake -m "${space}" -o otf-cff2 --output-dir build/otf --keep-overlaps

		pyftsubset "build/otf/${base}.otf" --output-file="${base}.otf" --layout-features='' --unicodes="${codes}"

		git -C "${dir}" clean -fd
	fi

	touchFileFromGitDir "${dir}" "${base}.otf"
}

generateFont Roboto Regular https://github.com/googlefonts/roboto-3-classic.git \
'U+0020-007E,U+00A4,U+00A7,U+00A9,U+00AB,U+00AE-00A5,U+00A7,U+00AE-00AF,U+00A9,U+00B0,U+00B4-00B5,U+00B7,U+00BB,U+00C6,U+00E6,00F8,U+03B1-03B2,U+2013-2014,U+201C-201D,U+2022,U+2026,U+202F,U+2122'

generateFont NotoSansSymbols Regular https://github.com/notofonts/symbols.git \
'U+2192'
