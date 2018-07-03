#ifndef __MCU_H
#define __MCU_H

#include "typedefs.h"

// ������� ���������
typedef enum
{
    // �� ������� ��������� HSE ��������� ��� �� �����
    MCU_HALT_REASON_RCC,
    // �� ������� ��������� LSE ��������� ��� �� �����
    MCU_HALT_REASON_RTC,
    
    // �� ������������� ����������
    MCU_HALT_REASON_IRQ,
    // ���������� Hard Fault
    MCU_HALT_REASON_SYS,
    // ���������� Memory Fault
    MCU_HALT_REASON_MEM,
    // ���������� Bus Fault
    MCU_HALT_REASON_BUS,
    // ���������� Usage Fault
    MCU_HALT_REASON_USG
} mcu_halt_reason_t;

// ������������� ������
void mcu_init(void);
// ���������� ��������� ��������� ����������
__noreturn void mcu_halt(mcu_halt_reason_t reason);

// ������� �� ������ ��� �������
void mcu_debug_pulse(void);

// ��������� ������� ��� ��������
void mcu_reg_update_32(volatile uint32_t *reg, uint32_t value_bits, uint32_t valid_bits);

#endif // __MCU_H
