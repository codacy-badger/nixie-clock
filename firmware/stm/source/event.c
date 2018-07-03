#include "event.h"
#include "nvic.h"
#include "system.h"

// ����� ���������� ���������� ��������
#define EVENT_TIMER_COUNT               10
// ���������� ����������� �� ���
#define EVENT_TIMER_US_PER_TICK         4
// ���������� ������� ������� �������
#define EVENT_TIMER_FREQUENCY_HZ        (MUL_M(1) / EVENT_TIMER_US_PER_TICK)

// �������� ��� ������� �� ���������� �������
#define EVENT_TIMER_NOT_FOUND           UINT8_MAX

// �������� ���������� (��������)
#if EVENT_TIMER_COUNT >= EVENT_TIMER_NOT_FOUND
    #error software timer count too large
#endif

// ����� �������� ������� ��� ����������� �������
#define EVENT_TIMER_PERIOD_RAW          ((event_t::timer_t::period_t)-1)
// ����������� ������ � �����
#define EVENT_TIMER_PERIOD_MIN          ((event_t::timer_t::period_t)2)
// ������������ ������ � �����
#define EVENT_TIMER_PERIOD_MAX          (EVENT_TIMER_PERIOD_RAW - EVENT_TIMER_PERIOD_MIN)

// ���������� ��������� �������
#define EVENT_TIMER_STATE_DECLARE(n)    MASK(event_t::timer_t::state_t, 1, n)
#define EVENT_TIMER_STATE_NONE          ((event_t::timer_t::state_t)0)  // ��������� �����������
#define EVENT_TIMER_STATE_ACTIVE        EVENT_TIMER_STATE_DECLARE(0)    // ������ �������
#define EVENT_TIMER_STATE_PENDING       EVENT_TIMER_STATE_DECLARE(1)    // ������ ��������
#define EVENT_TIMER_STATE_CIRQ          EVENT_TIMER_STATE_DECLARE(2)    // ��������� ������� �� ����������
// ��������� ��������� �������
#define EVENT_TIMER_STATE_USED          (EVENT_TIMER_STATE_ACTIVE | EVENT_TIMER_STATE_PENDING)

// ���������� ������ �������
struct event_item_controller_t
{
    // ��������� ����� �������� ���������
    static bool pending_set(event_item_t &item);
    // ����� �������� ���������
    static void pending_reset(event_item_t &item);
    // ��������� �������
    static void execute(event_item_t &item);
private:
    // �������� �����������
    event_item_controller_t(void)
    { }
};

// ����� ���������� �������
struct event_t
{
    // ������ ��� ���������� ��������
    struct timer_t
    {
        // ��� ������ ��� �������� ������� � �����
        typedef uint16_t period_t;
    private:
        // �������� ��������� �������
        typedef uint8_t state_t;
        
        // �������� �������
        struct slot_t
        {
            // �������� ����������
            event_interval_t current, reload;
            // ���������� �������
            delegate_t delegate;
            // ���������
            state_t state;
            
            // ����������� �� ���������
            slot_t(void) : state(EVENT_TIMER_STATE_NONE)
            { }
        } slot[EVENT_TIMER_COUNT];
        // ��������� �������� ����������� ��������
        period_t counter;
        // ������� ������ �������
        event_item_t item;
        
        // ����� ���������� ������� � ������ ������ (��������� �� ������� ������������)
        uint8_t find_free(void) const;
        // ����� ���������� ������� � ����� ������
        uint8_t find_free_back(void) const;
        // ����� ������� �� �����������
        uint8_t find_delegate(const delegate_t &delegate) const;
    public:
        // ����������� �� ���������
        timer_t(void) : item(DELEGATE_METHOD(this, event_t::timer_t, tick_handler)), counter(0)
        { }
        
        // ���������� ����� ��������
        void tick_handler(void);
        // ���������� ���������� ����������� �������
        void interrupt_handler(void);
        // ��������� ������ �������� ��� �������� �������/���������
        void ccr_set(period_t delta) const;
                
        // ����� ������������ ������� �� ��������� ���������� �����
        void start(const delegate_t &delegate, event_interval_t interval, event_timer_flag_t flags);
        // ���� ������������ ������� (���������� - ������� �� ������)
        bool stop(const delegate_t &delegate);
    } timer;
    
    // ������ ����� �������
    class scope_t
    {
        event_item_t *list[2];
        event_item_t **active;
    public:
        // �����������
        scope_t(void)
        {
            active = list;
            list[0] = list[1] = NULL;
        }
        
        // ���������� ������� � �������
        void add(event_item_t &item);
        // ���������� ������� ������� (���������� - ����� �� ��� ��� ����������)
        bool execute(void);
    } scope;    
};

// ��������� ������
static event_t event;

event_item_t::event_item_t(delegate_t _delegate) : pending(false), delegate(_delegate) 
{ }

INLINE_FORCED
bool event_item_controller_t::pending_set(event_item_t &item)
{
    // ���������� ������ ����
    assert(item.delegate.ready());
    // ���� ��� ���������� - �������
    if (item.pending)
        return false;
    // ������������
    item.pending = true;
    return true;
}

INLINE_FORCED
void event_item_controller_t::pending_reset(event_item_t &item)
{
    item.pending = false;
}

INLINE_FORCED
void event_item_controller_t::execute(event_item_t &item)
{
    // �������� ���������
    assert(item.delegate.ready());
    assert(item.pending);
    // ��������� �������
    item.delegate();
}

INLINE_FORCED
void event_t::timer_t::tick_handler(void)
{
    INTERRUPTS_SAVE();
    // ��������� ����� ��������
    for (uint8_t i = 0; i < EVENT_TIMER_COUNT; i++)
        if (slot[i].state & EVENT_TIMER_STATE_PENDING)
        {
            // ���������� ����
            slot[i].delegate();
            // ������ ��������� �������� ���������
            INTERRUPTS_DISABLE();
                slot[i].state &= ~EVENT_TIMER_STATE_PENDING;
            INTERRUPTS_RESTORE();
        }
}

OPTIMIZE_SPEED
void event_t::timer_t::ccr_set(period_t delta) const
{
    delta += (period_t)TIM3->CNT;
    // ��������� �������� CCR1
    TIM3->CCR1 = delta;
    // ����� ����� ����������
    TIM3->SR &= ~TIM_SR_CC1IF;                                                  // Clear IRQ CC1 pending flag
}

uint8_t event_t::timer_t::find_free(void) const
{
    for (uint8_t i = 0; i < EVENT_TIMER_COUNT; i++)
        if (!(slot[i].state & EVENT_TIMER_STATE_USED))
            return i;
    return EVENT_TIMER_NOT_FOUND;
}

uint8_t event_t::timer_t::find_free_back(void) const
{
    for (uint8_t i = EVENT_TIMER_COUNT; i > 0;)
        if (!(slot[--i].state & EVENT_TIMER_STATE_USED))
            return i;
    return EVENT_TIMER_NOT_FOUND;
}

uint8_t event_t::timer_t::find_delegate(const delegate_t &delegate) const
{
    for (uint8_t i = 0; i < EVENT_TIMER_COUNT; i++)
        if (slot[i].delegate == delegate)
            return i;
    return EVENT_TIMER_NOT_FOUND;
}

void event_t::timer_t::start(const delegate_t &delegate, event_interval_t interval, event_timer_flag_t flags)
{
    // �������� ����������
    assert(delegate.ready());
    if (interval <= 0)
        return;
    // ��������� ��� ����������
    INTERRUPT_SAFE_ENTER();
        // ����� �� �����������
        uint8_t i = find_delegate(delegate);
        if (i == EVENT_TIMER_NOT_FOUND)
        {
            // ����� ���������� �����
            i = (flags & EVENT_TIMER_FLAG_HEAD) ? find_free() : find_free_back();
            // ����� ���� �� ������ - ��� �����������
            assert(i != EVENT_TIMER_NOT_FOUND);
        }
        // ��������� �����
            // ����������, ��������
        slot[i].current = interval;
        slot[i].delegate = delegate;
            // ��������� (��� ����� ��������)
        slot[i].state |= EVENT_TIMER_STATE_ACTIVE;
        if (flags & EVENT_TIMER_FLAG_CIRQ)
            slot[i].state |= EVENT_TIMER_STATE_CIRQ;
        else
            slot[i].state &= ~EVENT_TIMER_STATE_CIRQ;
            // ������
        slot[i].reload = (flags & EVENT_TIMER_FLAG_LOOP) ? interval : 0;
    // �������������� ����������
    INTERRUPT_SAFE_LEAVE();
    // ������������ ������������ ����������
    ccr_set(EVENT_TIMER_PERIOD_MIN);
}

INLINE_FORCED
bool event_t::timer_t::stop(const delegate_t &delegate)
{
    bool result = false;
    // �������� ����������
    assert(delegate.ready());
    // ��������� ��� ����������
    INTERRUPT_SAFE_ENTER();
        // ����� �����
        uint8_t i = find_delegate(delegate);
        if (i != EVENT_TIMER_NOT_FOUND)
        {
            // ����������
            slot[i].state &= ~EVENT_TIMER_STATE_ACTIVE;
            result = true;
        }
    // �������������� ����������
    INTERRUPT_SAFE_LEAVE();
    return result;
}

INLINE_FORCED
OPTIMIZE_SPEED
void event_t::timer_t::interrupt_handler(void)
{
    bool event_gen = false;
    period_t dx, dt, ccr = EVENT_TIMER_PERIOD_MAX;
    // ����������� ������� ������� � �����
    dx = (period_t)TIM3->CNT;
    dx -= counter;
    counter += dx;
    // ��������� ��������
    for (uint8_t i = 0; i < EVENT_TIMER_COUNT; i++)
    {
        // ���� ������ �������� - �������
        if (!(slot[i].state & EVENT_TIMER_STATE_ACTIVE))
            continue;
        if (slot[i].current <= dx)
        {
            // ������������� �������
            if (slot[i].state & EVENT_TIMER_STATE_CIRQ)
                // ...����� �� ����������
                slot[i].delegate();
            else
            {
                // ...� �������� ����
                event_gen = true;
                slot[i].state |= EVENT_TIMER_STATE_PENDING;
            }
            if (slot[i].reload <= 0)
            {
                // ����������
                slot[i].state &= ~EVENT_TIMER_STATE_ACTIVE;
                continue;
            }
            // ���������� ������� ������� ����������
            dt = dx - slot[i].current;
            // ������������ ���������� ������� �� �������� ������������
            while (dt >= slot[i].reload)
                dt -= slot[i].reload;
            // ���������� ��������� ��������� ������������
            slot[i].current = slot[i].reload;
            // �������� ����������������� ���������� �����
            slot[i].current -= dt;
        }
        else
            slot[i].current -= dx;
        // ����������� ������� ���������� ������������ ����������� �������
        if (ccr > slot[i].current)
            ccr = slot[i].current;
    }
    // ������� ������� ���������� ������������
    if (ccr < EVENT_TIMER_PERIOD_MIN)
        ccr = EVENT_TIMER_PERIOD_MIN;
    // ��������� �������� CCR1
    ccr_set(ccr);
    // ��������� �������
    if (event_gen)
        event_add(item);
}

INLINE_FORCED
void event_t::scope_t::add(event_item_t &item)
{
    // ���������� � ������ ������ �������
    INTERRUPT_SAFE_ENTER();
        // ���� ������� ��� � �������� - �������
        if (event_item_controller_t::pending_set(item))
        {
            item.push(*active);
            *active = &item;
        }
    INTERRUPT_SAFE_LEAVE();
}

INLINE_FORCED
bool event_t::scope_t::execute(void)
{
    // ���������, ������ �� ������
    if (*active == NULL)
        return false;
    uint8_t i;
    INTERRUPT_SAFE_ENTER();
        // ���������� ����� ������ ��� �������
        if (*active == list[0])
            i = 0;
        else if (*active == list[1])
            i = 1;
        // �������� �������� ������� ������
        active = list + (i ^ 1);
        assert(*active == NULL);
    INTERRUPT_SAFE_LEAVE();
    // ��������� �������
    event_item_t *prev, *item = list[i];
    while (item != NULL)
    {
        // ��������� �������
        event_item_controller_t::execute(*item);
        // ������� �� ������
        prev = item;
        item = (event_item_t *)item->remove();
        // C��������� ���� ��������
        INTERRUPTS_DISABLE();
            event_item_controller_t::pending_reset(*prev);
        INTERRUPTS_RESTORE();
    }
    list[i] = NULL;
    // ����������, ���� �� ����� ������� �� ���������
    return *active != NULL;
}

void event_init(void)
{
    // ������������ � ����� �������
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;                                         // TIM3 clock enable
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;                                      // TIM3 reset
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;                                     // TIM3 unreset
    // ���������������� �������
    TIM3->CCMR1 = TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;                          // CC1 output, CC1 Fast off, CC1 preload off, CC1 mode Toggle (011)
    TIM3->PSC = FMCU_NORMAL_HZ / EVENT_TIMER_FREQUENCY_HZ - 1;                  // Prescaler (frequency)
    TIM3->ARR = EVENT_TIMER_PERIOD_RAW;                                         // Max period
    TIM3->DIER = TIM_DIER_CC1IE;                                                // CC1 IRQ enable
    // ���������������� ����������
    nvic_irq_enable_set(TIM3_IRQn, true);                                       // TIM3 IRQ enable
    nvic_irq_priority_set(TIM3_IRQn, NVIC_IRQ_PRIORITY_HIGHEST);                // Middle TIM3 IRQ priority
    // ����� ����������� �������
    event.timer.ccr_set(EVENT_TIMER_PERIOD_MAX);
    TIM3->CR1 |= TIM_CR1_CEN;                                                   // TIM enable
}

void event_add(event_item_t &item)
{
    event.scope.add(item);
}

bool event_execute(void)
{
    return event.scope.execute();
}

bool event_timer_stop(const delegate_t delegate)
{
    return event.timer.stop(delegate);
}

void event_timer_start_us(const delegate_t &delegate, event_interval_t us, event_timer_flag_t flags)
{
    event.timer.start(delegate, us / EVENT_TIMER_US_PER_TICK, flags);
}

void event_timer_start_hz(const delegate_t &delegate, float_t hz, event_timer_flag_t flags)
{
    assert(hz > 0.0f);
    assert(hz <= EVENT_TIMER_FREQUENCY_HZ);
    event.timer.start(delegate, (event_interval_t)(EVENT_TIMER_FREQUENCY_HZ / hz), flags);
}

IRQ_HANDLER
void event_interrupt_timer(void)
{
    event.timer.interrupt_handler();
}
