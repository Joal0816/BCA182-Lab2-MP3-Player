#include "rtos_timer_interface.h"

#ifndef NATIVE_TEST

/* Timeout implementation */
Timeout::Timeout() : m_timer(nullptr), m_cb(nullptr) {}

Timeout::~Timeout() {
    detach();
    if (m_timer) {
        xTimerDelete(m_timer, portMAX_DELAY);
        m_timer = nullptr;
    }
}

void Timeout::timer_thunk(TimerHandle_t xTimer) {
    Timeout* self = static_cast<Timeout*>(pvTimerGetTimerID(xTimer));
    if (self && self->m_cb) {
        self->m_cb();
    }
}

void Timeout::attach(timer_callback_fn callback, float delay_s) {
    uint32_t delay_ms = static_cast<uint32_t>(delay_s * 1000.0f);
    attach_ms(callback, delay_ms);
}

void Timeout::attach_ms(timer_callback_fn callback, uint32_t delay_ms) {
    detach();
    m_cb = callback;
    TickType_t ticks = pdMS_TO_TICKS(delay_ms);
    if (ticks == 0) ticks = 1;

    if (!m_timer) {
        m_timer = xTimerCreate("Timeout", ticks, pdFALSE, this, timer_thunk);
    } else {
        xTimerChangePeriod(m_timer, ticks, portMAX_DELAY);
    }

    if (m_timer) {
        xTimerStart(m_timer, portMAX_DELAY);
    }
}

void Timeout::detach() {
    if (m_timer && xTimerIsTimerActive(m_timer)) {
        xTimerStop(m_timer, portMAX_DELAY);
    }
}

bool Timeout::is_active() const {
    return m_timer && xTimerIsTimerActive(m_timer);
}

/* Ticker implementation */
Ticker::Ticker() : m_timer(nullptr), m_cb(nullptr) {}

Ticker::~Ticker() {
    detach();
    if (m_timer) {
        xTimerDelete(m_timer, portMAX_DELAY);
        m_timer = nullptr;
    }
}

void Ticker::timer_thunk(TimerHandle_t xTimer) {
    Ticker* self = static_cast<Ticker*>(pvTimerGetTimerID(xTimer));
    if (self && self->m_cb) {
        self->m_cb();
    }
}

void Ticker::attach(timer_callback_fn callback, float period_s) {
    uint32_t period_ms = static_cast<uint32_t>(period_s * 1000.0f);
    attach_ms(callback, period_ms);
}

void Ticker::attach_ms(timer_callback_fn callback, uint32_t period_ms) {
    detach();
    m_cb = callback;
    TickType_t ticks = pdMS_TO_TICKS(period_ms);
    if (ticks == 0) ticks = 1;

    if (!m_timer) {
        m_timer = xTimerCreate("Ticker", ticks, pdTRUE, this, timer_thunk);
    } else {
        xTimerChangePeriod(m_timer, ticks, portMAX_DELAY);
    }

    if (m_timer) {
        xTimerStart(m_timer, portMAX_DELAY);
    }
}

void Ticker::detach() {
    if (m_timer && xTimerIsTimerActive(m_timer)) {
        xTimerStop(m_timer, portMAX_DELAY);
    }
}

bool Ticker::is_active() const {
    return m_timer && xTimerIsTimerActive(m_timer);
}

#else

/* Native test stubs */
Timeout::Timeout() : m_cb(nullptr), m_active(false) {}
Timeout::~Timeout() {}
void Timeout::attach(timer_callback_fn callback, float delay_s) { (void)delay_s; m_cb = callback; m_active = true; }
void Timeout::attach_ms(timer_callback_fn callback, uint32_t delay_ms) { (void)delay_ms; m_cb = callback; m_active = true; }
void Timeout::detach() { m_active = false; }
bool Timeout::is_active() const { return m_active; }

Ticker::Ticker() : m_cb(nullptr), m_active(false) {}
Ticker::~Ticker() {}
void Ticker::attach(timer_callback_fn callback, float period_s) { (void)period_s; m_cb = callback; m_active = true; }
void Ticker::attach_ms(timer_callback_fn callback, uint32_t period_ms) { (void)period_ms; m_cb = callback; m_active = true; }
void Ticker::detach() { m_active = false; }
bool Ticker::is_active() const { return m_active; }

#endif
