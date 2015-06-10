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
    average = static_cast<float>(totalSum) / numSpeeds;
}

int DownloadTimeCalculator::getTime(int remainingSize)
{
    if (numSpeeds == 0) return 0;
    return remainingSize / average;
}



