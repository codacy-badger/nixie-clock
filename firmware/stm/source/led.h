#ifndef __LED_H
#define __LED_H

#include "hmi.h"

// ���������� �����������
#define LED_COUNT               HMI_RANK_COUNT

// ����� ������� �������� �����������
struct led_filter_chain_t : hmi_filter_chain_t<hmi_rgb_t, LED_COUNT>
{ };

// ����� ������� ��������� ����� ��� �����������
struct led_filter_gamma_t : hmi_filter_gamma_t<hmi_rgb_t, LED_COUNT>
{
protected:
    // ������� ����� ������
    virtual void do_data_set(hmi_rank_t index, hmi_rgb_t &data);
};

// ������� �������� ��� �����������
extern led_filter_chain_t led;
// ������ ��������� ����� ��� �����������
extern led_filter_gamma_t led_filter_gamma;

// ������������� ������
void led_init(void);

// ���������� DMA
void led_interrupt_dma(void);

#endif // __LED_H
