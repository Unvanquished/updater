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

DownloadTimeCalculator::DownloadTimeCalculator() :
    currentIndex_(0), average_(0), numSpeeds_(0), speeds_{0}
{
}

void DownloadTimeCalculator::addSpeed(int speed)
{
    int oldSpeed = speeds_[currentIndex_ % kMaxNumSpeeds];
    float totalSum = average_ * numSpeeds_;
    totalSum -= oldSpeed;
    if (numSpeeds_ < kMaxNumSpeeds) numSpeeds_++;
    speeds_[currentIndex_++ % kMaxNumSpeeds] = speed;
    totalSum += speed;
    average_ = totalSum / numSpeeds_;
}

int DownloadTimeCalculator::getTime(int remainingSize)
{
    if (numSpeeds_ == 0) return 0;
    if (remainingSize <= 0) return 0;
    return remainingSize / average_;
}
