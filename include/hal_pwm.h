/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file hal_pwm.h
 * @brief Pulse Width Modulation (PWM) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_PWM_H
#define CAFFEINE_HAL_HAL_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
#include "hal_gpio.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_PWM_POLARITY_NORMAL,
    HAL_PWM_POLARITY_INVERTED
} hal_pwm_polarity_t;

typedef enum
{
    HAL_PWM_INTERRUPT_PERIOD_MATCH,
    HAL_PWM_INTERRUPT_PULSE_MATCH,
    HAL_PWM_INTERRUPT_FAULT
} hal_pwm_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void      *timer_instance;
    uint32_t   channel;
    hal_gpio_t pin;
    void      *user_arg;
} hal_pwm_phy_t;

typedef struct
{
    uint32_t frequency_hz;
    uint32_t duty_cycle_percent; /* Usually 0 to 100, or a finer scale if
                                    hardware supports it */
    hal_pwm_polarity_t polarity;
    void              *custom;
} hal_pwm_config_t;

typedef struct hal_pwm_s hal_pwm_t;

typedef void (*hal_pwm_callback_t) (hal_pwm_t *driver, hal_pwm_interrupts_t interrupt, void *user_arg);

typedef struct hal_pwm_api_s hal_pwm_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_pwm_api_s
{
    hal_error_code_t (*hal_pwm_init) (hal_pwm_t *driver);
    hal_error_code_t (*hal_pwm_deinit) (hal_pwm_t *driver);
    hal_error_code_t (*hal_pwm_start) (hal_pwm_t *driver);
    hal_error_code_t (*hal_pwm_stop) (hal_pwm_t *driver);
    hal_error_code_t (*hal_pwm_set_frequency) (hal_pwm_t *driver, uint32_t frequency_hz);
    hal_error_code_t (*hal_pwm_set_duty_cycle) (hal_pwm_t *driver, uint32_t duty_percent);
    hal_error_code_t (*hal_pwm_register_cb) (hal_pwm_t *driver, hal_pwm_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_pwm_cfg_irq_enable) (hal_pwm_t *driver, hal_pwm_interrupts_t irq);
    hal_error_code_t (*hal_pwm_cfg_irq_disable) (hal_pwm_t *driver, hal_pwm_interrupts_t irq);
};

HAL_CREATE_DRIVER_TYPE (pwm, hal_pwm_config_t, hal_pwm_api_t, hal_pwm_phy_t, hal_pwm_callback_t);

/* Functions inline  ---------------------------------------------*/

/**
 * @brief hal_pwm_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_init (hal_pwm_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_PWM;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_init, driver, error);
    return error;
}

/**
 * @brief hal_pwm_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_deinit (hal_pwm_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_pwm_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_start (hal_pwm_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_start, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_pwm_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_stop (hal_pwm_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_stop, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_pwm_set_frequency implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param frequency_hz The target frequency in Hertz.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_set_frequency (hal_pwm_t *driver, uint32_t frequency_hz)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_set_frequency, driver, error, frequency_hz);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_pwm_set_duty_cycle implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param duty_percent The target duty cycle percentage (0-100).
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_set_duty_cycle (hal_pwm_t *driver, uint32_t duty_percent)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_set_duty_cycle, driver, error, duty_percent);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_pwm_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_register_cb (hal_pwm_t *driver, hal_pwm_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_pwm_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_cfg_irq_enable (hal_pwm_t *driver, hal_pwm_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_pwm_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_pwm_cfg_irq_disable (hal_pwm_t *driver, hal_pwm_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_PWM, hal_pwm_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_PWM_H
