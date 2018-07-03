#ifndef __DELEGATE_H
#define __DELEGATE_H

#include "typedefs.h"

/* ����� �������� �������� ��� ����������.
 * �������������� ��� ����� ����������� �������,
 * ��� � ����� ������ */
class delegate_t
{
    // ��������� �� ������, ����� ���� NULL
    void * context;
    // ��������� �� ���������� ����� ��� �������
    union
    {
        notify_proc_ptr proc;
        notify_method_ptr method;
    };
public:
    // ����������� �� ���������
    delegate_t(void) : context(NULL), method(NULL)
    { }
    // ����������� ��� ���������
    delegate_t(notify_proc_ptr routine) : context(NULL), proc(routine)
    {
        assert(routine != NULL);
    }
    // ����������� ��� ������
    delegate_t(notify_method_ptr routine, void *ctx) : context(ctx), method(routine)
    {
        assert(ctx != NULL);
        assert(routine != NULL);
    }

    // ���������
    bool operator == (const delegate_t& a) const
    { 
        return context == a.context && 
               method == a.method;
    }
    // ����������
    delegate_t& operator = (const delegate_t& a)
    {
        context = a.context;
        method = a.method;
        return *this;
    }
    // �����
    void operator ()(void) const
    {
        // �������� ����������
        assert(ready());
        // ���� ����� ����� ��������
        if (context != NULL)
            (((object_dummy_t *)(context))->*method)();
        else
            proc();
    }
    // ��������, ���� �� �������
    bool ready(void) const
    {
        return method != NULL; 
    }
};

// --- ������� ������������� �������� --- //

// ��� ���������
#define DELEGATE_PROC(proc)                 delegate_t((notify_proc_ptr)(proc))
// ��� ����� � ����� ��������� ������
#define DELEGATE_METHOD(ref, type, method)  delegate_t((notify_method_ptr)&type::method, (void *)(ref))
// ��� ����� � ��������� �������
#define DELEGATE_METHOD_VAR(var, method)    DELEGATE_METHOD(&(var), decltype(var), method)
// ��� ����� � ��������� ������� �� ���������
#define DELEGATE_METHOD_PTR(ref, method)    DELEGATE_METHOD_VAR(*(ref), method)

#endif // __DELEGATE_H
