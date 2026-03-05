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
 * @file hal_rtc.h
 * @brief Real-Time Clock (RTC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_RTC_H
#define CAFFEINE_HAL_HAL_RTC_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include <time.h>
#include "hal_types.h"
#include "hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_RTC_INTERRUPT_ALARM,
    HAL_RTC_INTERRUPT_WAKEUP,

    HAL_RTC_INTERRUPT_TAMPER,
    HAL_RTC_INTERRUPT_TIMESTAMP,
    HAL_RTC_INTERRUPT_ERROR
} hal_rtc_interrupts_t;

typedef enum
{
    HAL_RTC_FORMAT_12H,
    HAL_RTC_FORMAT_24H,

    HAL_RTC_FORMAT_TOTAL
} hal_rtc_format_t;

typedef enum
{
    HAL_RTC_MODE_BCD,
    HAL_RTC_MODE_BINARY,
    HAL_RTC_MODE_BINARY_BCD,

    HAL_RTC_MODE_TOTAL
} hal_rtc_mode_t;

/* Types Structs ----------------------------------------------------*/
typedef struct tm hal_rtc_time_t;

typedef struct
{
    void *port;
    void *user_arg;
} hal_rtc_phy_t;

typedef struct
{
    hal_rtc_format_t format;
    hal_rtc_mode_t   mode;
    uint32_t         target_rtc_clock;
    void            *custom;
} hal_rtc_config_t;

typedef struct hal_rtc_s hal_rtc_t;

typedef struct hal_rtc_api_s hal_rtc_api_t;

typedef void (*hal_rtc_callback_t) (hal_rtc_t *driver, hal_rtc_interrupts_t interrupt, uint32_t id, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_rtc_api_s
{
    hal_error_code_t (*hal_rtc_register_cb) (hal_rtc_t *driver, hal_rtc_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_rtc_set_cb_arg) (hal_rtc_t *driver, void *user_arg);
    hal_error_code_t (*hal_rtc_init) (hal_rtc_t *driver);
    hal_error_code_t (*hal_rtc_deinit) (hal_rtc_t *driver);
    hal_error_code_t (*hal_rtc_cfg_get) (hal_rtc_t *driver, hal_rtc_config_t *config);
    hal_error_code_t (*hal_rtc_cfg_set) (hal_rtc_t *driver, const hal_rtc_config_t *config);
    hal_error_code_t (*hal_rtc_check_error) (hal_rtc_t *driver);
    hal_error_code_t (*hal_rtc_set_time) (hal_rtc_t *driver, hal_rtc_time_t *time);
    hal_error_code_t (*hal_rtc_get_time) (hal_rtc_t *driver, hal_rtc_time_t *time);
    hal_error_code_t (*hal_rtc_set_alarm) (hal_rtc_t *driver, uint32_t id, hal_rtc_time_t *time, hal_rtc_callback_t cb);
    hal_error_code_t (*hal_rtc_get_alarm) (hal_rtc_t *driver, uint32_t id, bool *elapsed, hal_rtc_time_t *time,
                                           hal_rtc_callback_t cb);
    hal_error_code_t (*hal_rtc_stop_alarm) (hal_rtc_t *driver, uint32_t id);
    hal_error_code_t (*hal_rtc_cfg_irq_enable) (hal_rtc_t *driver, hal_rtc_interrupts_t irq);
    hal_error_code_t (*hal_rtc_cfg_irq_disable) (hal_rtc_t *driver, hal_rtc_interrupts_t irq);
};

HAL_CREATE_DRIVER_TYPE (rtc, hal_rtc_config_t, hal_rtc_api_t, hal_rtc_phy_t, hal_rtc_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_rtc_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_init (hal_rtc_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_RTC;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_init, driver, error);
    return error;
}
/**
 * @brief hal_rtc_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_deinit (hal_rtc_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}
/**
 * @brief hal_rtc_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_cfg_get (hal_rtc_t *driver, hal_rtc_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_cfg_get, driver, error, config);
    return error;
}
/**
 * @brief hal_rtc_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_cfg_set (hal_rtc_t *driver, const hal_rtc_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_cfg_set, driver, error, config);
    return error;
}
/**
 * @brief hal_rtc_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_register_cb (hal_rtc_t *driver, hal_rtc_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_rtc_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_set_cb_arg (hal_rtc_t *driver, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief hal_rtc_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_check_error (hal_rtc_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_check_error, driver, error);
    return error;
}

/**
 * @brief hal_rtc_set_time implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param time Pointer to the RTC time structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_set_time (hal_rtc_t *driver, hal_rtc_time_t *time)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_set_time, driver, error, time);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_rtc_get_time implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param time Pointer to the RTC time structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_get_time (hal_rtc_t *driver, hal_rtc_time_t *time)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_get_time, driver, error, time);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_rtc_set_alarm implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param id Alarm or resource identifier.
 * @param time Pointer to the RTC time structure.
 * @param cb Callback function to register.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_set_alarm (hal_rtc_t *driver, uint32_t id, hal_rtc_time_t *time,
                                                  hal_rtc_callback_t cb)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_set_alarm, driver, error, id, time, cb);
    HAL_UNLOCK (driver);
    return error;
}

static inline hal_error_code_t hal_rtc_get_alarm (hal_rtc_t *driver, uint32_t id, bool *elapsed, hal_rtc_time_t *time,
                                                  hal_rtc_callback_t cb)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_get_alarm, driver, error, id, elapsed, time, cb);
    return error;
}

/**
 * @brief hal_rtc_stop_alarm implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param id Alarm or resource identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_stop_alarm (hal_rtc_t *driver, uint32_t id)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_stop_alarm, driver, error, id);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_rtc_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_cfg_irq_enable (hal_rtc_t *driver, hal_rtc_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_rtc_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_rtc_cfg_irq_disable (hal_rtc_t *driver, hal_rtc_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_RTC, hal_rtc_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_RTC_H
