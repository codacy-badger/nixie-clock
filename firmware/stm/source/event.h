#ifndef __EVENT_H
#define __EVENT_H

#include "list.h"
#include "delegate.h"

// ��� ������ ��� �������� ��������� � �����
typedef uint32_t event_interval_t;

// ���������� ������ �������
struct event_item_controller_t;

// ����� �������
class event_item_t : public list_item_t
{
    friend event_item_controller_t;
    // ���������, ��� ������� �������� � ������� ���������
    bool pending;
    // ���������� �������
    delegate_t delegate;
public:
    // ����������� �� ���������
    event_item_t(delegate_t _delegate);
};

// ������������� ������ (���������� �����)
void event_init(void);
// ���������� ������� ������� (���������� - ����� �� ��� ��� ����������)
bool event_execute(void);
// ���������� ������� � �������
void event_add(event_item_t &item);

// ����� ��� �������� �������
typedef uint8_t event_timer_flag_t;
#define EVENT_TIMER_FLAG_DECLARE(n)     MASK(event_timer_flag_t, 1, n)
#define EVENT_TIMER_FLAG_NONE           ((event_timer_flag_t)0)     // ����� ������ �����
#define EVENT_TIMER_FLAG_LOOP           EVENT_TIMER_FLAG_DECLARE(0) // ������ �����������
#define EVENT_TIMER_FLAG_HEAD           EVENT_TIMER_FLAG_DECLARE(1) // �������������� � ������ �������
#define EVENT_TIMER_FLAG_CIRQ           EVENT_TIMER_FLAG_DECLARE(2) // ���������� �� ����������

// ���������� ��������� �������
#define EVENT_TIMER_PRI_DEFAULT         (EVENT_TIMER_FLAG_NONE)
#define EVENT_TIMER_PRI_HIGHEST         (EVENT_TIMER_FLAG_HEAD)
#define EVENT_TIMER_PRI_CRITICAL        (EVENT_TIMER_FLAG_HEAD | EVENT_TIMER_FLAG_CIRQ)

// ����� ������������ �������
void event_timer_start_us(const delegate_t &handler, event_interval_t us, event_timer_flag_t flags = EVENT_TIMER_FLAG_NONE);
void event_timer_start_hz(const delegate_t &handler, float_t hz, event_timer_flag_t flags = EVENT_TIMER_FLAG_NONE);
// ���� ������������ ������� (���������� - ������� �� ������)
bool event_timer_stop(const delegate_t handler);

// ���������� ���������� ����������� �������
void event_interrupt_timer(void);

#endif // __EVENT_H
