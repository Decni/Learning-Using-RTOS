/*************************************** Copyright (c)******************************************************
** File name            :   tHooks.c
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS的Hooks扩展接口
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   01课堂 lishutong
** Created date         :   2016-06-01
** Version              :   1.0
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   版权所有，禁止用于商业用途
** Author Blog          :   http://ilishutong.com
**********************************************************************************************************/
#include "tHooks.h"
#include "tinyOS.h"

#if TINYOS_ENABLE_HOOKS == 1

void tHooksCpuIdle (void) {

}

void tHooksSysTick (void) {

}

void tHooksTaskSwitch (tTask *from, tTask *to) {

}

void tHooksTaskInit (tTask *task) {

}

#endif
