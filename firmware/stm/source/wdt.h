#ifndef __WDT_H
#define __WDT_H

#include "typedefs.h"

// ������������� ������
void wdt_init(void);
// ����� �������
void wdt_pulse(void);
// ��������� �������
void wdt_stop(void);

#endif // __WDT_H
