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
 * @file cfn_hal_pwm.h
 * @brief Pulse Width Modulation (PWM) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_PWM_H
#define CAFFEINE_HAL_HAL_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_gpio.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_PWM_POLARITY_NORMAL,
    CFN_HAL_PWM_POLARITY_INVERTED
} cfn_hal_pwm_polarity_t;

typedef enum
{
    CFN_HAL_PWM_INTERRUPT_PERIOD_MATCH,
    CFN_HAL_PWM_INTERRUPT_PULSE_MATCH,
    CFN_HAL_PWM_INTERRUPT_FAULT
} cfn_hal_pwm_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void          *timer_instance;
    uint32_t       channel;
    cfn_hal_gpio_t pin;
    void          *user_arg;
} cfn_hal_pwm_phy_t;

typedef struct
{
    uint32_t frequency_hz;
    uint32_t duty_cycle_percent; /* Usually 0 to 100, or a finer scale if
                                    hardware supports it */
    cfn_hal_pwm_polarity_t polarity;
    void                  *custom;
} cfn_hal_pwm_config_t;

typedef struct cfn_hal_pwm_s cfn_hal_pwm_t;

typedef void (*cfn_hal_pwm_callback_t)(cfn_hal_pwm_t *driver, cfn_hal_pwm_interrupts_t interrupt, void *user_arg);

typedef struct cfn_hal_pwm_api_s cfn_hal_pwm_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_pwm_api_s
{
    cfn_hal_error_code_t (*cfn_hal_pwm_init)(cfn_hal_pwm_t *driver);
    cfn_hal_error_code_t (*cfn_hal_pwm_deinit)(cfn_hal_pwm_t *driver);
    cfn_hal_error_code_t (*cfn_hal_pwm_start)(cfn_hal_pwm_t *driver);
    cfn_hal_error_code_t (*cfn_hal_pwm_stop)(cfn_hal_pwm_t *driver);
    cfn_hal_error_code_t (*cfn_hal_pwm_set_frequency)(cfn_hal_pwm_t *driver, uint32_t frequency_hz);
    cfn_hal_error_code_t (*cfn_hal_pwm_set_duty_cycle)(cfn_hal_pwm_t *driver, uint32_t duty_percent);
    cfn_hal_error_code_t (*cfn_hal_pwm_register_cb)(cfn_hal_pwm_t *driver, cfn_hal_pwm_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_pwm_cfg_irq_enable)(cfn_hal_pwm_t *driver, cfn_hal_pwm_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_pwm_cfg_irq_disable)(cfn_hal_pwm_t *driver, cfn_hal_pwm_interrupts_t irq);
};

/**
 * @brief Generated driver structure for pwm.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_pwm_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_pwm_config_t *config;
 *     const cfn_hal_pwm_api_t    *api;
 *     const cfn_hal_pwm_phy_t    *phy;
 *     cfn_hal_pwm_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_pwm_s cfn_hal_pwm_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(pwm, cfn_hal_pwm_config_t, cfn_hal_pwm_api_t, cfn_hal_pwm_phy_t, cfn_hal_pwm_callback_t);

/* Functions inline  ---------------------------------------------*/

/**
 * @brief cfn_hal_pwm_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_init(cfn_hal_pwm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    if (driver && driver->base.on_config)
    {
        error = driver->base.on_config(&driver->base, DRIVER_CONFIG_INIT);
        if (error != CFN_HAL_ERROR_OK)
        {
            return error;
        }
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_pwm_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_start(cfn_hal_pwm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_start, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_pwm_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_stop(cfn_hal_pwm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_stop, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_pwm_set_frequency implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param frequency_hz The target frequency in Hertz.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_set_frequency(cfn_hal_pwm_t *driver, uint32_t frequency_hz)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_set_frequency, driver, error, frequency_hz);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_pwm_set_duty_cycle implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param duty_percent The target duty cycle percentage (0-100).
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_set_duty_cycle(cfn_hal_pwm_t *driver, uint32_t duty_percent)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_set_duty_cycle, driver, error, duty_percent);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_pwm_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_pwm_register_cb(cfn_hal_pwm_t *driver, cfn_hal_pwm_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_pwm_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_cfg_irq_enable(cfn_hal_pwm_t *driver, cfn_hal_pwm_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_pwm_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pwm_cfg_irq_disable(cfn_hal_pwm_t *driver, cfn_hal_pwm_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_PWM, cfn_hal_pwm_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_PWM_H
