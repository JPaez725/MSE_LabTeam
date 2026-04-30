/**
 * @file    pwm.h
 * @brief   PWM output module – TIM2 Channel 1, pin PA0 (AF1).
 *
 * Hardware mapping (fixed):
 *   Timer : TIM2, Channel 1  (APB1 general-purpose, no BDTR needed)
 *   Pin   : PA0  →  AF1 = TIM2_CH1
 *   Clock : 16 MHz HSI
 *
 * pwm_set_signal() writes CCR directly — no preload delay.
 * Duty cycle range: 0–100 %.
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void pwm_init(uint32_t frequency_hz);
void pwm_set_signal(uint8_t duty_cycle_pct);
void pwm_start(void);
void pwm_stop(void);

#endif /* PWM_H */