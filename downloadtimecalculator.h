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
