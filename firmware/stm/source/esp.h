#ifndef __ESP_H
#define __ESP_H

#include "typedefs.h"

// ������������� ������
void esp_init(void);

// SPI ����������
void esp_transaction(void);

// ���������� DMA
void esp_interrupt_dma(void);

#endif // __ESP_H
