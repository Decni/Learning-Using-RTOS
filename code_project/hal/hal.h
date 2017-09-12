/**
 * @brief tOS 目标相关配置代码
 * @details
 *     tOS是一个小巧的嵌入式操作系统，为方便学习使用而开发。所有代码全部自行开发，本着简单易学的原则,
 * 所有设计尽可能采用比较简单的设计。在设计之初，选择了目前最常用的ARM Cortex-M3内核为目标内核进行
 * 设计，无论你使用的是哪一款具体的Cortex-M芯片，均可能很方便地进行移植。
 *     由于该RTOS主要用于教学，没有经过严格的验证测试，所以不保证可靠性和稳定性。再次说明，本系统主要用于
 * 学习理解RTOS的工作原理。如有Bug，欢迎访问我的博客：http://ilishutong.com
 *     本工程主要用于演示tOS各个模块如何使用。通过学习，你可以顺利地理解如何使用其它RTOS。
 *     如果你对tOS的实现原理和应用感兴趣，也欢迎访问我的博客，找到相关的配套教学视频。
 *
 * @author 01课堂 lishutong
 * @date 2017-06-01
 * @version 1.0
 * @copyright 版权所有，禁止用于商业用途
 */
#ifndef PROJECT_TARGET_H
#define PROJECT_TARGET_H

#include <stm32f10x.h>
#include "tinyOS.h"

#if TINYOS_ENABLE_MUTEX == 1
    extern tMutex xprintfMutex;

    #define xprintf(fmt, ...) {     \
            tMutexWait(&xprintfMutex, 0);   \
            printf(fmt, ##__VA_ARGS__);       \
            tMutexNotify(&xprintfMutex);    \
    }
#else
    #define xprintf(fmt, ...) { printf(fmt, ##__VA_ARGS__); }
#endif


void targetInit (void);

void targetEnterSleep (void);

void interruptByOtherTask (void);

void interruptEnable (IRQn_Type irq, int enable);

void interruptByInterrupt (IRQn_Type irq);

void xprintfMem (uint8_t * mem, uint32_t size);

#endif //PROJECT_TARGET_H
