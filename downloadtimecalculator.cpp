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

#include "downloadtimecalculator.h"

DownloadTimeCalculator::DownloadTimeCalculator() : currentIndex(0), average(0), numSpeeds(0), speeds{0}
{
}

void DownloadTimeCalculator::addSpeed(int speed)
{
    int oldSpeed = speeds[currentIndex % kMaxNumSpeeds];
    float totalSum = average * numSpeeds;
    totalSum -= oldSpeed;
    if (numSpeeds < kMaxNumSpeeds) numSpeeds++;
    speeds[currentIndex++ % kMaxNumSpeeds] = speed;
    totalSum += speed;
    average = totalSum / numSpeeds;
}

int DownloadTimeCalculator::getTime(int remainingSize)
{
    if (numSpeeds == 0) return 0;
    if (remainingSize <= 0) return 0;
    return remainingSize / average;
}
