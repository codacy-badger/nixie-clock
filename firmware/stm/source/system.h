#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "typedefs.h"
#include <stm32f1xx.h>

/* ������������ ���������:
   rtc - RTC
   led - TIM1 (CH3), DMA1 (CH6)
   wdt - IWDG (2 Hz)
   event - TIM3 (CH1)
   tube - TIM4 (CH2), TIM2 (CH3)
   esp - SPI1 (master), DMA1 (CH2, CH3)
*/

// ������� ���� ��� ������
#define FMCU_STARTUP_MHZ    8
#define FMCU_STARTUP_KHZ    MUL_K(FMCU_STARTUP_MHZ)
#define FMCU_STARTUP_HZ     MUL_M(FMCU_STARTUP_MHZ)
// ������� ���� ����� �������������
#define FMCU_NORMAL_MHZ     96
#define FMCU_NORMAL_KHZ     MUL_K(FMCU_NORMAL_MHZ)
#define FMCU_NORMAL_HZ      MUL_M(FMCU_NORMAL_MHZ)

// ������� LSI [��]
#define FLSI_KHZ            40
#define FLSI_HZ             MUL_K(FLSI_KHZ)

// ��� �������� �����
#define SEGMENT_STACK       "CSTACK"
// ��� �������� ������� �������� ����������
#define SEGMENT_VTBL        ".intvec"

// ��������/��������� ������ �� ������ � ����� ����� 
#define BKP_ACCESS_ALLOW()  PWR->CR |= PWR_CR_DBP                               // Disable backup domain write protection
#define BKP_ACCESS_DENY()   PWR->CR &= ~PWR_CR_DBP;                             // Enable backup domain write protection

#endif // __SYSTEM_H
