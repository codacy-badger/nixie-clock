#ifndef __IO_H
#define __IO_H

#include "typedefs.h"

// �������� �������� ����, � ����� �� ���������� ��������!
typedef struct
{
	// ��������� ������������� ����
	uint8_t index;
	// �������� ���������
	bool inverse, state;
	// ������ ��� �������
	os_timer_t blink_timer;
} gpio_t;

// ������������� �������� ����
void gpio_setup(gpio_t *ref, uint8_t index, uint32_t mux, uint8_t func, bool inverse);
// ��������� ��������� ��������� ����
void gpio_state_set(gpio_t *ref, bool state);
// ��������� ��������� ������� ����
void gpio_blink_set(gpio_t *ref, bool state);

#endif // __IO_H
