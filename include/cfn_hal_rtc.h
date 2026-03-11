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
 * @file cfn_hal_rtc.h
 * @brief Real-Time Clock (RTC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_RTC_H
#define CAFFEINE_HAL_HAL_RTC_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_RTC_INTERRUPT_ALARM,
    CFN_HAL_RTC_INTERRUPT_WAKEUP,

    CFN_HAL_RTC_INTERRUPT_TAMPER,
    CFN_HAL_RTC_INTERRUPT_TIMESTAMP,
    CFN_HAL_RTC_INTERRUPT_ERROR
} cfn_hal_rtc_interrupts_t;

typedef enum
{
    CFN_HAL_RTC_FORMAT_12H,
    CFN_HAL_RTC_FORMAT_24H,

    CFN_HAL_RTC_FORMAT_TOTAL
} cfn_hal_rtc_format_t;

typedef enum
{
    CFN_HAL_RTC_MODE_BCD,
    CFN_HAL_RTC_MODE_BINARY,
    CFN_HAL_RTC_MODE_BINARY_BCD,

    CFN_HAL_RTC_MODE_TOTAL
} cfn_hal_rtc_mode_t;

/* Types Structs ----------------------------------------------------*/
typedef struct tm cfn_hal_rtc_time_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_rtc_phy_t;

typedef struct
{
    cfn_hal_rtc_format_t format;
    cfn_hal_rtc_mode_t   mode;
    uint32_t             target_rtc_clock;
    void                *custom;
} cfn_hal_rtc_config_t;

typedef struct cfn_hal_rtc_s cfn_hal_rtc_t;

typedef struct cfn_hal_rtc_api_s cfn_hal_rtc_api_t;

typedef void (*cfn_hal_rtc_callback_t)(
    cfn_hal_rtc_t *driver, cfn_hal_rtc_interrupts_t interrupt, uint32_t id, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_rtc_api_s
{
    cfn_hal_error_code_t (*cfn_hal_rtc_register_cb)(cfn_hal_rtc_t *driver, cfn_hal_rtc_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_rtc_set_cb_arg)(cfn_hal_rtc_t *driver, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_rtc_init)(cfn_hal_rtc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_rtc_deinit)(cfn_hal_rtc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_rtc_cfg_get)(cfn_hal_rtc_t *driver, cfn_hal_rtc_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_rtc_cfg_set)(cfn_hal_rtc_t *driver, const cfn_hal_rtc_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_rtc_check_error)(cfn_hal_rtc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_rtc_set_time)(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time);
    cfn_hal_error_code_t (*cfn_hal_rtc_get_time)(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time);
    cfn_hal_error_code_t (*cfn_hal_rtc_set_alarm)(
        cfn_hal_rtc_t *driver, uint32_t id, cfn_hal_rtc_time_t *time, cfn_hal_rtc_callback_t cb);
    cfn_hal_error_code_t (*cfn_hal_rtc_get_alarm)(
        cfn_hal_rtc_t *driver, uint32_t id, bool *elapsed, cfn_hal_rtc_time_t *time, cfn_hal_rtc_callback_t cb);
    cfn_hal_error_code_t (*cfn_hal_rtc_stop_alarm)(cfn_hal_rtc_t *driver, uint32_t id);
    cfn_hal_error_code_t (*cfn_hal_rtc_cfg_irq_enable)(cfn_hal_rtc_t *driver, cfn_hal_rtc_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_rtc_cfg_irq_disable)(cfn_hal_rtc_t *driver, cfn_hal_rtc_interrupts_t irq);
};

/**
 * @brief Generated driver structure for rtc.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_rtc_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_rtc_config_t *config;
 *     const cfn_hal_rtc_api_t    *api;
 *     const cfn_hal_rtc_phy_t    *phy;
 *     cfn_hal_rtc_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_rtc_s cfn_hal_rtc_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(rtc, cfn_hal_rtc_config_t, cfn_hal_rtc_api_t, cfn_hal_rtc_phy_t, cfn_hal_rtc_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief cfn_hal_rtc_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_init(cfn_hal_rtc_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}
/**
 * @brief cfn_hal_rtc_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_cfg_get(cfn_hal_rtc_t *driver, cfn_hal_rtc_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_cfg_get, driver, error, config);
    return error;
}
/**
 * @brief cfn_hal_rtc_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_cfg_set(cfn_hal_rtc_t *driver, const cfn_hal_rtc_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_cfg_set, driver, error, config);
    return error;
}
/**
 * @brief cfn_hal_rtc_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_rtc_register_cb(cfn_hal_rtc_t *driver, cfn_hal_rtc_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_rtc_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_set_cb_arg(cfn_hal_rtc_t *driver, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief cfn_hal_rtc_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_check_error(cfn_hal_rtc_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_check_error, driver, error);
    return error;
}

/**
 * @brief cfn_hal_rtc_set_time implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param time Pointer to the RTC time structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_set_time(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_set_time, driver, error, time);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_rtc_get_time implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param time Pointer to the RTC time structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_get_time(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_get_time, driver, error, time);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_rtc_set_alarm implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param id Alarm or resource identifier.
 * @param time Pointer to the RTC time structure.
 * @param cb Callback function to register.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_rtc_set_alarm(cfn_hal_rtc_t *driver, uint32_t id, cfn_hal_rtc_time_t *time, cfn_hal_rtc_callback_t cb)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_set_alarm, driver, error, id, time, cb);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_rtc_get_alarm(
    cfn_hal_rtc_t *driver, uint32_t id, bool *elapsed, cfn_hal_rtc_time_t *time, cfn_hal_rtc_callback_t cb)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_get_alarm, driver, error, id, elapsed, time, cb);
    return error;
}

/**
 * @brief cfn_hal_rtc_stop_alarm implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param id Alarm or resource identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_stop_alarm(cfn_hal_rtc_t *driver, uint32_t id)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_stop_alarm, driver, error, id);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_rtc_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_cfg_irq_enable(cfn_hal_rtc_t *driver, cfn_hal_rtc_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_rtc_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_rtc_cfg_irq_disable(cfn_hal_rtc_t *driver, cfn_hal_rtc_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, cfn_hal_rtc_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_RTC_H
