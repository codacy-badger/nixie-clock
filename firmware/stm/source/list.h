#ifndef __LIST_H
#define __LIST_H

#include "typedefs.h"

// ������� ���������� ����������������� ������
class list_item_t
{
    // ������ �� ��������� �������
    list_item_t *next;
public:
    // ����������� �� ���������
    list_item_t() : next(NULL) { }
    
    // �������� ������ �� ��������� �������
    INLINE_FORCED
    list_item_t * next_get(void) const
    { return next; };
    // ������� �������� ������ ����� place
    void insert(list_item_t *place = NULL);
    // ���������� �������� �� ������ ����� place
    list_item_t * remove(list_item_t *place = NULL);
    // ����������� ���� ��������� ������, ������� � ��������
    void uncouple(void);

    // ������� �������� ������ �� place
    void push(list_item_t *place = NULL);
    // �������� ��������� ������� ������
    list_item_t * last_get(void);
    // ������� �������� � ����� ������
    void last_push(list_item_t &item);
};

#endif // __LIST_H
