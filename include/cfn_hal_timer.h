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
 * @file cfn_hal_timer.h
 * @brief TIMER Hardware Abstraction Layer.
 */

#ifndef CAFFEINE_HAL_HAL_TIMER_H
#define CAFFEINE_HAL_HAL_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"

/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_TIMER_INTERRUPT_UPDATE,
    CFN_HAL_TIMER_INTERRUPT_CC1,
    CFN_HAL_TIMER_INTERRUPT_CC2,
    CFN_HAL_TIMER_INTERRUPT_CC3,
    CFN_HAL_TIMER_INTERRUPT_CC4,
    CFN_HAL_TIMER_INTERRUPT_TRIGGER,
    CFN_HAL_TIMER_INTERRUPT_BREAK,
    CFN_HAL_TIMER_INTERRUPT_ERROR,
} cfn_hal_timer_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    uint32_t prescaler;
    uint32_t period;
    void    *custom;
} cfn_hal_timer_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_timer_phy_t;

typedef struct
{
    uint32_t channel;
    uint64_t ticks;
} cfn_hal_timer_period_t;

typedef struct cfn_hal_timer_s cfn_hal_timer_t;

typedef struct cfn_hal_timer_api_s cfn_hal_timer_api_t;

typedef void (*cfn_hal_timer_callback_t)(
    cfn_hal_timer_t *driver, cfn_hal_timer_interrupts_t interrupt, uint32_t channel, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_timer_api_s
{
    cfn_hal_error_code_t (*cfn_hal_timer_start)(cfn_hal_timer_t *driver);
    cfn_hal_error_code_t (*cfn_hal_timer_stop)(cfn_hal_timer_t *driver);
    cfn_hal_error_code_t (*cfn_hal_timer_get_ticks)(cfn_hal_timer_t *driver, uint32_t ch, uint32_t *ticks);
    cfn_hal_error_code_t (*cfn_hal_timer_get_ticks_u64)(cfn_hal_timer_t *driver, uint32_t ch, uint64_t *ticks);
    cfn_hal_error_code_t (*cfn_hal_timer_set_period)(cfn_hal_timer_t *driver, const cfn_hal_timer_period_t *period);
    cfn_hal_error_code_t (*cfn_hal_timer_init)(cfn_hal_timer_t *driver);
    cfn_hal_error_code_t (*cfn_hal_timer_deinit)(cfn_hal_timer_t *driver);
    cfn_hal_error_code_t (*cfn_hal_timer_cfg_get)(cfn_hal_timer_t *driver, cfn_hal_timer_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_timer_cfg_set)(cfn_hal_timer_t *driver, const cfn_hal_timer_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_timer_cfg_irq_enable)(cfn_hal_timer_t *driver, cfn_hal_timer_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_timer_cfg_irq_disable)(cfn_hal_timer_t *driver, cfn_hal_timer_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_timer_register_cb)(
        cfn_hal_timer_t *driver, cfn_hal_timer_callback_t cb, void *user_arg);
};

/**
 * @brief Generated driver structure for timer.
 */
CFN_HAL_CREATE_DRIVER_TYPE(
    timer, cfn_hal_timer_config_t, cfn_hal_timer_api_t, cfn_hal_timer_phy_t, cfn_hal_timer_callback_t);

/* Functions prototypes ---------------------------------------------*/
static inline cfn_hal_error_code_t cfn_hal_timer_start(cfn_hal_timer_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_start, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_stop(cfn_hal_timer_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_stop, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_get_ticks(cfn_hal_timer_t *driver, uint32_t ch, uint32_t *ticks)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_get_ticks, driver, error, ch, ticks);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_get_ticks_u64(cfn_hal_timer_t *driver, uint32_t ch, uint64_t *ticks)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_get_ticks_u64, driver, error, ch, ticks);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t
cfn_hal_timer_set_period(cfn_hal_timer_t *driver, const cfn_hal_timer_period_t *period)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_set_period, driver, error, period);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_init(cfn_hal_timer_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_deinit(cfn_hal_timer_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_deinit, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    }
    if (driver && driver->base.on_config)
    {
        driver->base.on_config(&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_cfg_get(cfn_hal_timer_t *driver, cfn_hal_timer_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_cfg_get, driver, error, config);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_cfg_set(cfn_hal_timer_t *driver, const cfn_hal_timer_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_cfg_set, driver, error, config);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_timer_cfg_irq_enable(cfn_hal_timer_t *driver, cfn_hal_timer_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_cfg_irq_enable, driver, error, irq);
    return error;
}

static inline cfn_hal_error_code_t
cfn_hal_timer_cfg_irq_disable(cfn_hal_timer_t *driver, cfn_hal_timer_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_cfg_irq_disable, driver, error, irq);
    return error;
}

static inline cfn_hal_error_code_t
cfn_hal_timer_register_cb(cfn_hal_timer_t *driver, cfn_hal_timer_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_TIMER, cfn_hal_timer_register_cb, driver, error, cb, user_arg);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TIMER_H
