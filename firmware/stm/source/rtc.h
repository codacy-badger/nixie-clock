#ifndef __RTC_H
#define __RTC_H

#include "typedefs.h"

// ��������� �������� ����/�������
typedef struct
{
    // ����
    uint16_t year;
    uint8_t month;
    uint8_t day;
    // �����
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} rtc_datetime_t;

// ������������� ������
void rtc_init(void);
// ����� ������� RTC /64
void rtc_clock_output(bool enabled);

#endif // __RTC_H
