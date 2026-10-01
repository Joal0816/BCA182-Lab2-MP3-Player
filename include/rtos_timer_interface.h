#ifndef RTOS_TIMER_INTERFACE_H
#define RTOS_TIMER_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifndef NATIVE_TEST
#include "FreeRTOS.h"
#include "timers.h"
#endif

typedef void (*timer_callback_fn)(void);

/**
 * @brief Timeout interface (runs a callback once after a delay, matching mbed OS Timeout API).
 * Implemented on FreeRTOS software timers.
 */
class Timeout {
public:
    Timeout();
    ~Timeout();

    void attach(timer_callback_fn callback, float delay_s);
    void attach_ms(timer_callback_fn callback, uint32_t delay_ms);
    void detach();
    bool is_active() const;

private:
#ifndef NATIVE_TEST
    TimerHandle_t m_timer;
    timer_callback_fn m_cb;
    static void timer_thunk(TimerHandle_t xTimer);
#else
    timer_callback_fn m_cb;
    bool m_active;
#endif
};

/**
 * @brief Ticker interface (repeatedly calls a callback at a specified interval, matching mbed OS Ticker API).
 * Implemented on FreeRTOS software timers.
 */
class Ticker {
public:
    Ticker();
    ~Ticker();

    void attach(timer_callback_fn callback, float period_s);
    void attach_ms(timer_callback_fn callback, uint32_t period_ms);
    void detach();
    bool is_active() const;

private:
#ifndef NATIVE_TEST
    TimerHandle_t m_timer;
    timer_callback_fn m_cb;
    static void timer_thunk(TimerHandle_t xTimer);
#else
    timer_callback_fn m_cb;
    bool m_active;
#endif
};

#endif /* RTOS_TIMER_INTERFACE_H */
