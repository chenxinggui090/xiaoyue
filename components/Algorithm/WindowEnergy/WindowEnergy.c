#include "WindowEnergy.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

float temp_signal[SIGNAL_LENGTH];  // 输入信号数组
int Crack = 0;

void sliding_window_peak_detection(double Noise) {
    int n = SIGNAL_LENGTH;
    int ii = 0;
    // int Crack = 0;
    float max_val111 = 0;
    // 动态分配内存
    float *buff = (float *)malloc(SW * sizeof(float));
    if (buff == NULL) {
        printf("Memory allocation failed for buff\n");
        return;
    }

    float *buff_previous_window = (float *)malloc(SW * sizeof(float));
    if (buff_previous_window == NULL) {
        printf("Memory allocation failed for buff_previous_window\n");
        free(buff);
        return;
    }

    float *buff_next_window = (float *)malloc(SW * sizeof(float));
    if (buff_next_window == NULL) {
        printf("Memory allocation failed for buff_next_window\n");
        free(buff);
        free(buff_previous_window);
        return;
    }
    float sum = 0;
    for (size_t i = 0; i < SIGNAL_LENGTH; ++i)
    {
        // sum += abs(audioData[i]);
        sum = (sum + fabs(temp_signal[i]))/2;
    }
    // printf("sum = %f\r\n",sum);

    float energy1 = 0;
    int crack_flag = 0;
    for (int i = 0; i < n; i++) {
        if (temp_signal[i] > Noise*3) {
            crack_flag = 1;
            break;
        }
    }

    if (crack_flag) {
        if (sum < Noise*n*1.5) Crack++;
    }

    
    // while (ii <= n - SW) {
    //     // 提取当前滑动窗口数据
    //     for (int i = 0; i < SW; i++) {
    //         buff[i] = fabs(temp_signal[ii + i]);
    //     }

    //     // 直接找到最大值及其索引（避免排序）
    //     int index = 0;
    //     float max_val = buff[0];
    //     for (int i = 1; i < SW; i++) {
    //         if (buff[i] > max_val) {
    //             max_val = buff[i];
    //             index = i;
    //         }
    //     }

    //     if (ii > 1 * SW) {
    //         // 条件一：幅度最大值大于底噪的两倍  
    //         if(max_val111<max_val)
    //         {
    //             max_val111 = max_val;
    //         }
    //         if (max_val >= Noise * 1.5) {
    //             printf("max_val:%f, Noise * 1,5:%f\r\n", max_val,Noise * 1.5);
    //             // 向前推一个窗口
    //             // float buff_previous_window[SW];
    //             for (int i = 0; i < SW; i++) {
    //                 buff_previous_window[i] = fabs(temp_signal[ii - SW + i]);
    //             }

    //             // 向后推一个窗口
    //             // float buff_next_window[SW];
    //             for (int i = 0; i < SW; i++) {
    //                 buff_next_window[i] = fabs(temp_signal[ii + SW + i]);
    //             }

    //             // 查找前后窗口的最大值
    //             float max_previous = buff_previous_window[0];
    //             for (int i = 1; i < SW; i++) {
    //                 if (buff_previous_window[i] > max_previous) {
    //                     max_previous = buff_previous_window[i];
    //                 }
    //             }

    //             float max_next = buff_next_window[0];
    //             for (int i = 1; i < SW; i++) {
    //                 if (buff_next_window[i] > max_next) {
    //                     max_next = buff_next_window[i];
    //                 }
    //             }
    //             // printf("----------------------------\n");
    //             // 条件二：相邻三个窗口内的最大值
    //             if (max_next <= max_val && max_previous <= max_val) {
    //                 // 计算能量
    //                 float energy = 0;
    //                 for (int i = 0; i < SW; i++) {
    //                     energy += temp_signal[ii + i] * temp_signal[ii + i];
    //                 }
    //                 printf("Energy: %f\n", energy);

    //                 // 条件三：短时窗口能量超过设定阈值
    //                 if (energy < Threshold) {
    //                     Crack++;
    //                     printf("Crack count: %d\n", Crack);
    //                     ii = ii + SW + 1;  // 跳过一个窗口
    //                 } else {
    //                     ii = ii + SW + 1;  // 跳过一个窗口
    //                 }
    //             } else {
    //                 // 更新最大值为候选特征信号
    //                 index = ii + SW;
    //                 ii = index; // 跳到候选特征信号位置
    //             }
    //         } else {
    //             ii++;
    //         }
    //     } else {
    //         ii++;
    //     }
    // }
    printf("ADC_average2:%f,Crack:%d\r\n",sum,Crack);
    // 释放内存
    free(buff);
    free(buff_previous_window);
    free(buff_next_window);
}

