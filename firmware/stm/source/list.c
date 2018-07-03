#include "list.h"

void list_item_t::insert(list_item_t *place)
{
    list_item_t *ll;
    // �������� ����������
    assert(next == NULL);
    // ���������
    if (place == NULL)
        return;
    ll = place->next;
    place->next = this;
    next = ll;
}

list_item_t * list_item_t::remove(list_item_t *place)
{
    list_item_t *ll;
    // ����������
    ll = next;
    next = NULL;
    if (place == NULL)
        return ll;
    assert(place->next == this);
    place->next = ll;
    return ll;
}

void list_item_t::uncouple(void)
{
    list_item_t *head = this;
    // ����� ������
    do
    {
        head = head->remove();
    } while (head != NULL);
}

void list_item_t::push(list_item_t *place)
{
    // ���������
    next = place;
}

list_item_t * list_item_t::last_get(void)
{
    list_item_t *head = this;
    // ����� ������
    while (head->next != NULL)
        head = head->next;
    return head;
}

void list_item_t::last_push(list_item_t &item)
{
    // �������� ��������� ������� ������
    list_item_t *head = this->last_get();
    // �������
    item.insert(head);
}
