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

#ifndef DOWNLOADTIMECALCULATOR_H
#define DOWNLOADTIMECALCULATOR_H
#include <vector>

class DownloadTimeCalculator {
public:
    DownloadTimeCalculator();
    void addSpeed(int speed);
    int getTime(int remainingSize);

private:
    static const int kMaxNumSpeeds = 100;
    int currentIndex;
    float average;
    int numSpeeds;
    int speeds[kMaxNumSpeeds];
};
#endif
