#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H

// STD
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
// IAR
#include <intrinsics.h>

// ������ ������
#define MACRO_EMPTY

// ������ ���������
#ifndef NULL
    #define NULL                0
#endif

// ��������� �� 1000
#define MUL_K(v)                (1000ul * (v)) 
// ��������� �� 1000000
#define MUL_M(v)                (1000000ul * (v))
// ������� ���������� � ������������
#define MS_TO_US(v)             MUL_K(v)

// �������� ������� �����
#define MASK(type, value, pos)  ((type)(value) << (pos))
#define MASK_32(value, pos)     MASK(uint32_t, value, pos)

// ������ ������� � C-style
#ifdef __cplusplus
    #define EXTERN_C            extern "C"
#else
    #define EXTERN_C            MACRO_EMPTY
#endif

#define SAFE_BLOCK(code)        \
    do {                        \
        code;                   \
    } while (false)

// ������������ ���� ������
typedef float float32_t;
typedef double float64_t;
// ������������ ������������ ��� �� ���������
typedef float32_t float_t;

// �����-��������
class object_dummy_t
{ };

#define PRAGMA(msg)             _Pragma(_STRINGIFY_B(msg))
#define PRAGMA_OPTION(m, v)      PRAGMA(m = v)

// ����� �������������� ����������
#define UNUSED(x)   (void)(x)
// ������ ����������� �������
#define OPTIMIZE_NONE           PRAGMA_OPTION(optimize, none)
#define OPTIMIZE_SIZE           PRAGMA_OPTION(optimize, size)
#define OPTIMIZE_SPEED          PRAGMA_OPTION(optimize, speed)
// ������������ �������
#define INLINE_NEVER            PRAGMA_OPTION(inline, never)
#define INLINE_FORCED           PRAGMA_OPTION(inline, forced)

// ���������/���������� ��������������
#define WARNING_SUPPRESS(err)   PRAGMA_OPTION(diag_suppress, err)
#define WARNING_DEFAULT(err)    PRAGMA_OPTION(diag_default, err)

// ����������� ��� ����������
#define IRQ_HANDLER             INLINE_NEVER OPTIMIZE_SPEED

// ������������ ����� � ����������
#define FIELD_ALIGN_ONE         PRAGMA(pack(1))
#define FIELD_ALIGN_TWO         PRAGMA(pack(2))
#define FIELD_ALIGN_HOUR        PRAGMA(pack(4))
        
// ������������ ������
#define DATA_ALIGN_TWO          PRAGMA_OPTION(data_alignment, 2)
#define DATA_ALIGN_HOUR         PRAGMA_OPTION(data_alignment, 4)

// �������� FALSE
#define WAITWHILE(expr)         do { } while (expr)
// �������� TRUE
#define WAITFOR(expr)           WAITWHILE(!(expr))

// �������� ������
#define MEMORY_CLEAR(var)       memset(&(var), 0, sizeof(var))
// �������� ������ �� ���������
#define MEMORY_CLEAR_PTR(var)   memset(var, 0, sizeof(*(var)))

// ��������� ������� ��������� ����������
#define INTERRUPTS_SAVE()       __istate_t __isate = __get_interrupt_state()
// ���������� ����������� ��������� ����������
#define INTERRUPTS_RESTORE()    __set_interrupt_state(__isate)
// ��������� ��� ������������ ����������
#define INTERRUPTS_DISABLE()    __disable_interrupt()

// ����/����� � ����������  �� ���������� ���
#define INTERRUPT_SAFE_ENTER()  \
    INTERRUPTS_SAVE();          \
    INTERRUPTS_DISABLE()
#define INTERRUPT_SAFE_LEAVE()  INTERRUPTS_RESTORE()

// ������� � ������ ����� �� ����������
#define WFI()                   __WFI()
        
// �������� ��������� ����������
typedef void (* notify_proc_ptr)(void);

// �������� ������ ����������
typedef void (object_dummy_t::* notify_method_ptr)(void);

#endif // __TYPEDEFS_H
