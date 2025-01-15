#ifndef _WINDOWENERGY_H
#define _WINDOWENERGY_H

// #define Noise 1000 //底噪幅度 (需要优化成智能检测该值大小)
#define Threshold 1000 //能量阈值 (需要优化成智能检测该值大小)

#define SW 1000  // 滑动窗口大小0.02s *50KHz = 1000个样本
#define SAMPLE_RATE 50000  // 采样率 50KHz
#define SIGNAL_LENGTH 48000  // 假设信号长度（根据具体情况调整）

extern float temp_signal[SIGNAL_LENGTH]; 

void sliding_window_peak_detection(double Noise);
extern int Crack;
extern short buffffff;
#endif
