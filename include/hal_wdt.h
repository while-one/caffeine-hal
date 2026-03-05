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
 * @file hal_wdt.h
 * @brief Watchdog Timer (WDT) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_WDT_H
#define CAFFEINE_HAL_HAL_WDT_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_WDT_CFG_SLEEP_RUN,
    HAL_WDT_CFG_SLEEP_PAUSE
} hal_wdt_cfg_sleep_t;

typedef enum
{
    HAL_WDT_CFG_RESET_NONE,
    HAL_WDT_CFG_RESET_IRQ,
    HAL_WDT_CFG_RESET_CPU
} hal_wdt_cfg_reset_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    hal_wdt_cfg_sleep_t sleep;
    hal_wdt_cfg_reset_t reset;
    void               *user_arg;
} hal_wdt_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} hal_wdt_phy_t;

typedef struct hal_wdt_s hal_wdt_t;

typedef struct hal_wdt_api_s hal_wdt_api_t;

typedef void (*hal_wdt_callback_t) (hal_wdt_t *driver, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_wdt_api_s
{
    hal_error_code_t (*hal_wdt_register_cb) (hal_wdt_t *driver, hal_wdt_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_wdt_set_cb_arg) (hal_wdt_t *driver, void *user_arg);
    hal_error_code_t (*hal_wdt_check_error) (hal_wdt_t *driver);
    hal_error_code_t (*hal_wdt_init) (hal_wdt_t *driver);
    hal_error_code_t (*hal_wdt_deinit) (hal_wdt_t *driver);
    hal_error_code_t (*hal_wdt_cfg_get) (hal_wdt_t *driver, hal_wdt_config_t *config);
    hal_error_code_t (*hal_wdt_cfg_set) (hal_wdt_t *driver, const hal_wdt_config_t *config);
    hal_error_code_t (*hal_wdt_start) (hal_wdt_t *driver);
    hal_error_code_t (*hal_wdt_stop) (hal_wdt_t *driver);
    hal_error_code_t (*hal_wdt_feed) (hal_wdt_t *driver);
};

HAL_CREATE_DRIVER_TYPE (wdt, hal_wdt_config_t, hal_wdt_api_t, hal_wdt_phy_t, hal_wdt_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_wdt_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_register_cb (hal_wdt_t *driver, hal_wdt_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_wdt_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_set_cb_arg (hal_wdt_t *driver, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief hal_wdt_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_check_error (hal_wdt_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_check_error, driver, error);
    return error;
}

/**
 * @brief hal_wdt_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_init (hal_wdt_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_WDT;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_init, driver, error);
    return error;
}

/**
 * @brief hal_wdt_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_deinit (hal_wdt_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_wdt_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_cfg_get (hal_wdt_t *driver, hal_wdt_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_wdt_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_cfg_set (hal_wdt_t *driver, const hal_wdt_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_wdt_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_start (hal_wdt_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_start, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_wdt_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_stop (hal_wdt_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_stop, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_wdt_feed implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_wdt_feed (hal_wdt_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_WDT, hal_wdt_feed, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_WDT_H
