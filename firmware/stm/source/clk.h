#ifndef __CLK_H
#define __CLK_H

#include "system.h"

// �������� ������������ ������ �������
typedef enum
{
    CLK_MCO_SOURCE_NONE = RCC_CFGR_MCO_NOCLOCK,
    CLK_MCO_SOURCE_SYS = RCC_CFGR_MCO_SYSCLK,
    CLK_MCO_SOURCE_HSI = RCC_CFGR_MCO_HSI,
    CLK_MCO_SOURCE_HSE = RCC_CFGR_MCO_HSE,
    CLK_MCO_SOURCE_PLL = RCC_CFGR_MCO_PLLCLK_DIV2
} clk_mco_source_t;

// ������������� ��� ��� ������� ���������� �������
typedef uint32_t clk_period_ms_t;

// �������� ������� ������ ��� �������� � ���������
typedef bool (*clk_pool_handler_ptr)(void);

// ������������� ������
void clk_init(void);
// ����� ������� �� IO
void clk_mco_output(clk_mco_source_t source);
// �������� � ��, �� �������� �� ����������
void clk_delay(clk_period_ms_t ms = 1);
// ��������� ������� ������, ������ � ��, �� �������� �� ����������
bool clk_pool(clk_pool_handler_ptr proc, clk_period_ms_t ms = 1);

// ���������� ���������� ���������� �������
void clk_interrupt_systick(void);

#endif // __CLK_H
