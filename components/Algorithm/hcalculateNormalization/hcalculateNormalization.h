#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define NormalizationFactor   (1.0 / 32768)

double calculateNoise(int16_t *audioData, size_t dataSize);
