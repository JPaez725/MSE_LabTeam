/**
 * @file    pwm.c
 * @brief   PWM output – TIM2 Channel 1 on PA0 (AF1 = TIM2_CH1).
 *
 * What changed from the previous version and why:
 *
 *  1. Timer  : TIM1 → TIM2 (general-purpose, APB1, no BDTR.MOE needed).
 *  2. Pin    : PA8  → PA0  (PA0 = TIM2_CH1 via AF1, verified working).
 *  3. No OC preload (OC1PE removed) — CCR takes effect immediately.
 *  4. pwm_set_signal() reads ARR directly from the register so the
 *     CCR calculation is always consistent with the active period.
 *  5. Channel enable moved to pwm_start() (matches working code order).
 */

#include "pwm.h"
#include "tim_driver.h"
#include "gpio_driver.h"
#include "stm32f411xe.h"

#define PWM_PIN     (0U)    /* PA0 = TIM2_CH1 (AF1) */

/* ── pwm_init ────────────────────────────────────────────────────── */
void pwm_init(uint32_t frequency_hz)
{
    /* 1. Enable clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* 2. Configure PA0 as AF1 (TIM2_CH1) */
    gpio_init(GPIOA, PWM_PIN, GPIO_MODE_AF);
    gpio_set_output_type(GPIOA, PWM_PIN, GPIO_OTYPE_PP);
    gpio_set_speed(GPIOA, PWM_PIN, GPIO_SPEED_HIGH);
    gpio_set_pupd(GPIOA, PWM_PIN, GPIO_PUPD_NONE);
    gpio_set_af(GPIOA, PWM_PIN, GPIO_AF1);

    /* 3. Configure TIM2 frequency (calculates PSC/ARR automatically) */
    tim_set_frequency(TIM2, frequency_hz);

    /* 4. Configure CH1 in PWM Mode 1 (no OC preload) */
    tim_pwm_config(TIM2, TIM_CHANNEL_1, TIM_PWM_MODE_1);

    /* 5. Start with 0% duty cycle */
    tim_set_ccr(TIM2, TIM_CHANNEL_1, 0U);

    /* Channel enable and counter start happen in pwm_start() */
}

/* ── pwm_set_signal ──────────────────────────────────────────────── */
/**
 * @brief  Set duty cycle by reading ARR directly from the register.
 *         CCR1 = (ARR + 1) * duty / 100
 *         Because there is no preload, this takes effect on the next
 *         timer cycle — no UEV flush needed.
 */
void pwm_set_signal(uint8_t duty_cycle_pct)
{
    if (duty_cycle_pct > 100U) { duty_cycle_pct = 100U; }

    uint32_t arr    = tim_get_arr(TIM2);          /* Read live ARR register */
    uint32_t ccr    = ((arr + 1U) * (uint32_t)duty_cycle_pct) / 100U;

    tim_set_ccr(TIM2, TIM_CHANNEL_1, ccr);
}

/* ── pwm_start ───────────────────────────────────────────────────── */
void pwm_start(void)
{
    tim_channel_enable(TIM2, TIM_CHANNEL_1);  /* CCER.CC1E = 1 */
    tim_enable(TIM2);                          /* CR1.CEN   = 1 */
}

/* ── pwm_stop ────────────────────────────────────────────────────── */
void pwm_stop(void)
{
    tim_channel_disable(TIM2, TIM_CHANNEL_1);
    tim_disable(TIM2);
}