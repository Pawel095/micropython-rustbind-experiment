/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2020 Jim Mussared
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_MIMXRT_MPHALPORT_H
#define MICROPY_INCLUDED_MIMXRT_MPHALPORT_H

#include <stdint.h>
#include "ticks.h"
#include "py/ringbuf.h"
#include "pin.h"
#include "fsl_clock.h"

#define MICROPY_HAL_VERSION             "2.8.0"

#define MP_HAL_PIN_FMT                  "%q"
extern ringbuf_t stdin_ringbuf;

// Define an alias fo systick_ms, because the shared softtimer.c uses
// the symbol uwTick for the systick ms counter.
#define uwTick systick_ms

#define mp_hal_pin_obj_t const machine_pin_obj_t *
#define mp_hal_get_pin_obj(o)   pin_find(o)
#define mp_hal_pin_name(p)      ((p)->name)
#define mp_hal_pin_input(p) machine_pin_set_mode(p, PIN_MODE_IN);
#define mp_hal_pin_output(p) machine_pin_set_mode(p, PIN_MODE_OUT);
#define mp_hal_pin_open_drain(p) machine_pin_set_mode(p, PIN_MODE_OPEN_DRAIN);
#define mp_hal_pin_high(p) (p->gpio->DR_SET = 1 << p->pin)
#define mp_hal_pin_low(p) (p->gpio->DR_CLEAR = 1 << p->pin)
#define mp_hal_pin_write(p, value) (GPIO_PinWrite(p->gpio, p->pin, value))
#define mp_hal_pin_toggle(p) (GPIO_PortToggle(p->gpio, (1 << p->pin)))
#define mp_hal_pin_read(p) (GPIO_PinReadPadStatus(p->gpio, p->pin))

#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)

#define mp_hal_quiet_timing_enter() raise_irq_pri(1)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)

void mp_hal_set_interrupt_char(int c);

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return ticks_ms32();
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    return ticks_us32();
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    uint64_t us = (uint64_t)ms * 1000;
    ticks_delay_us64(us);
}

static inline void mp_hal_delay_us(mp_uint_t us) {
    ticks_delay_us64(us);
}

#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

static inline void mp_hal_ticks_cpu_enable(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->LAR = 0xc5acce55;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    return DWT->CYCCNT;
}

static inline mp_uint_t mp_hal_get_cpu_freq(void) {
    return CLOCK_GetCpuClkFreq();
}

enum {
    MP_HAL_MAC_WLAN0 = 0,
    MP_HAL_MAC_WLAN1,
    MP_HAL_MAC_BDADDR,
    MP_HAL_MAC_ETH0,
    MP_HAL_MAC_ETH1,
};

void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest);
void mp_hal_get_unique_id(uint8_t id[]);

#endif // MICROPY_INCLUDED_MIMXRT_MPHALPORT_H
