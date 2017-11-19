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
 * @author 01课堂 李述铜 http://01ketang.cc
 * @date 2017-06-01
 * @version 1.0
 * @copyright 版权所有，禁止用于商业用途
 */
#include <stdio.h>
#include "tinyOS.h"

#define RTE_DEVICE_STDPERIPH_USART

#include "stm32f10x_conf.h"
#include "hal.h"

#if TINYOS_ENABLE_MUTEX == 1
    tMutex xprintfMutex;
#endif

void usartInit (void) {
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

#if TINYOS_ENABLE_MUTEX == 1
    tMutexInit(&xprintfMutex);
#endif
}

void extioInit (void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    EXTI_ClearITPendingBit(EXTI_Line0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * 初始化目标设备
 */
void halInit (void) {
    usartInit();
    extioInit();
}

/**
 * 进入低功耗模式
 */
void targetEnterSleep (void) {
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}

/**
 * 串口发送数据
 * @param ch
 */
void uartSendChar (char ch) {
    USART_SendData(USART1, (unsigned char) ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
}

/**
 * 用于stm32f103 printf重定向到usart1
 */
int fputc (int ch, FILE *f) {
    uartSendChar(ch);
    return (ch);
}

/**
 * 通过延迟切换到其它任务，以模拟任务被突然强制中断的情况
 */
void interruptByOtherTask (void) {
    tTaskDelay(1);
}

/**
 * 使能指定的中断
 * @param irq 使能的中断序号
 */
void interruptEnable (IRQn_Type irq, int enable) {
    if (enable) {
        NVIC_EnableIRQ(irq);
    } else {
        NVIC_DisableIRQ(irq);
    }
}

/**
 * 通过设置IRQ挂起位，主动触发一个中断
 */
void interruptByIRQ (IRQn_Type irq) {
    NVIC_SetPendingIRQ(irq);
}

/**
 *
 * @param mem
 * @param size
 */
void printMem (uint8_t *mem, uint32_t size) {
    uint32_t i;
    uint32_t column = 0;

    for (i = 0; i < size; i++) {
        if (++column % 8 == 0) {
            xprintf("0x%x\n", mem[i]);
        } else {
            xprintf("0x%x,", mem[i]);
        }
    }
    xprintf("\n");
}
