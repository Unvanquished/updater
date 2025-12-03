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
'U+0020-007E,U+00A1-00A7,U+00A9,U+00AB-00AC,U+00AE-00AF,U+00B0-00B5,U+00B7,U+00B9-00BF,U+00C0-00DF,U+00E0-00FF,U+0152-0153,U+0391-03A1,U+03A3-03A9,U+03B1-03C9,U+202F,U+2010-2015,U+2018-201F,U+2022-2023,U+2026,U+203C-203D,U+2044,U+2047-2049,U+207B,U+20AC,U+2122,U+2160-216F,U+2212,U+221A,U+221E,U+2248,U+2260,U+2264-2265,U+2E2E'

generateFont NotoSansSymbols Regular https://github.com/notofonts/symbols.git \
'U+2192'
