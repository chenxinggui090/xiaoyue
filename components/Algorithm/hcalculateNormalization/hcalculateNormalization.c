#include "hcalculateNormalization.h"

// 函数：计算底噪
double calculateNoise(int16_t *audioData, size_t dataSize)
{
    // 计算均值
    int16_t sum = 0;
    for (size_t i = 0; i < dataSize; ++i)
    {
        sum = (sum + abs(audioData[i]))/2;
    }
    return ((double)sum) * NormalizationFactor;
}
