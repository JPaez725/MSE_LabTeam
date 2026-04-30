/**
 * @file    main.c
 * @brief   TIM driver demo – NUCLEO-F411RE.
 *
 * PA5 (LD2) : blinks every 500 ms via TIM5 blocking delay.
 * PA0       : breathe fade via TIM2 CH1 PWM (1 kHz, 0–100 % sweep).
 *
 * Timer allocation:
 *   TIM2 → PWM on PA0  (APB1, general-purpose, no BDTR needed)
 *   TIM5 → delays      (APB1, separate — never conflicts with TIM2)
 */

#include "stm32f411xe.h"
#include "led.h"
#include "timer.h"
#include "pwm.h"

#define PWM_FREQ_HZ     (1000U)   /* 1 kHz PWM carrier   */
#define FADE_STEP_MS    (100U)     /* 10 ms per duty step */
#define BLINK_MS        (500U)    /* LED half-period     */

int main(void)
{
    /* Initialise peripherals */
    led_init();                   /* PA5 → output                       */
    timer_init();                 /* TIM5 clock enable                  */
    pwm_init(PWM_FREQ_HZ);        /* TIM2 CH1 → PA0, AF1, 1 kHz         */
    pwm_start();                  /* Start counter + enable CH1 output  */

    while (1)
    {
        /* ── Fade up PA0 (0 → 100 %) ──────────────────────────── */
        for (uint8_t duty = 0U; duty <= 100U; duty += 10U)
        {
            pwm_set_signal(duty);
            timer_delay_ms(FADE_STEP_MS);
        }
        led_toggle();  /* Toggle PA5 once per fade up (every ~1 s) */
        /*── Fade down PA0 (100 → 0 %) ────────────────────────── */ 
        for (uint8_t duty = 100U; duty > 0U; duty -= 10U)
        {
            pwm_set_signal(duty);
            timer_delay_ms(FADE_STEP_MS);
        }
        pwm_set_signal(0U);

        /* ── Toggle PA5 once per fade down (~1 s) ───────── */
        led_toggle();
    }

    return 0;
}