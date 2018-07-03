#ifndef __NVIC_H
#define __NVIC_H

#include "system.h"

// ���������� ����������, �� ������� � ��������
typedef enum
{
    NVIC_IRQ_PRIORITY_0 = 15,
    NVIC_IRQ_PRIORITY_1 = 14,
    NVIC_IRQ_PRIORITY_2 = 13,
    NVIC_IRQ_PRIORITY_3 = 12,
    NVIC_IRQ_PRIORITY_4 = 11,
    NVIC_IRQ_PRIORITY_5 = 10,
    NVIC_IRQ_PRIORITY_6 = 9,
    NVIC_IRQ_PRIORITY_7 = 8,
    NVIC_IRQ_PRIORITY_8 = 7,
    NVIC_IRQ_PRIORITY_9 = 6,
    NVIC_IRQ_PRIORITY_10 = 5,
    NVIC_IRQ_PRIORITY_11 = 4,
    NVIC_IRQ_PRIORITY_12 = 3,
    NVIC_IRQ_PRIORITY_13 = 2,
    NVIC_IRQ_PRIORITY_14 = 1,
    NVIC_IRQ_PRIORITY_15 = 0
} nvic_irq_priority_t;

// ������ � ������� ��������� ����������
#define NVIC_IRQ_PRIORITY_LOWEST    NVIC_IRQ_PRIORITY_0
#define NVIC_IRQ_PRIORITY_HIGHEST   NVIC_IRQ_PRIORITY_15

// ������������� ������
void nvic_init(void);
// ��������� ���������� ����������
void nvic_irq_priority_set(IRQn_Type irq, nvic_irq_priority_t priority);
// �������� ��������� ����������
nvic_irq_priority_t nvic_irq_priority_get(IRQn_Type irq);

// ��������/��������� ����������
void nvic_irq_enable_set(IRQn_Type irq, bool state);

#endif // __NVIC_H
