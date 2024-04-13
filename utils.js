/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

.pragma library

function humanSize(bytes) {
    var sizes = ['Bytes', 'KiB', 'MiB', 'GiB'];
    var i = 0;
    var size = bytes;
    while (size > 1024 && i++ < sizes.length) {
        size /= 1024;
    }

    return '' + Number(size).toFixed(1) + ' ' + sizes[Math.min(i, sizes.length - 1)];
}

function padZeros(input, padding) {
    var str = input.toString();
    var paddingStr = '';
    if (str.length < padding) {
        for (var i = 0; i < padding - str.length; ++i)
            paddingStr += '0'
    }
    return paddingStr + str;
}

function humanTime(time) {
    var hours = Math.floor(time / 3600.0)
    var mins = Math.floor((time % 3600) / 60.0)
    var secs = time % 60.0
    return '' + padZeros(hours, 2) + ':' + padZeros(mins, 2) + ':' + padZeros(secs, 2) + ' left';
}

// Suitable for use outside tags
function htmlEscape(text) {
    return text.replace(/&/g, '&amp;')
               .replace(/</g, '&lt;')
               .replace(/>/g, '&gt;')
}
