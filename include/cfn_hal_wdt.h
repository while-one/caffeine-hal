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
 * @file cfn_hal_wdt.h
 * @brief Watchdog Timer (WDT) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_WDT_H
#define CAFFEINE_HAL_HAL_WDT_H

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
    CFN_HAL_WDT_CFG_SLEEP_RUN,
    CFN_HAL_WDT_CFG_SLEEP_PAUSE
} cfn_hal_wdt_cfg_sleep_t;

typedef enum
{
    CFN_HAL_WDT_CFG_RESET_NONE,
    CFN_HAL_WDT_CFG_RESET_IRQ,
    CFN_HAL_WDT_CFG_RESET_CPU
} cfn_hal_wdt_cfg_reset_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    cfn_hal_wdt_cfg_sleep_t sleep;
    cfn_hal_wdt_cfg_reset_t reset;
    void                   *user_arg;
} cfn_hal_wdt_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_wdt_phy_t;

typedef struct cfn_hal_wdt_s cfn_hal_wdt_t;

typedef struct cfn_hal_wdt_api_s cfn_hal_wdt_api_t;

typedef void (*cfn_hal_wdt_callback_t)(cfn_hal_wdt_t *driver, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_wdt_api_s
{
    cfn_hal_error_code_t (*cfn_hal_wdt_register_cb)(cfn_hal_wdt_t *driver, cfn_hal_wdt_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_wdt_set_cb_arg)(cfn_hal_wdt_t *driver, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_wdt_check_error)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*cfn_hal_wdt_init)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*cfn_hal_wdt_deinit)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*cfn_hal_wdt_cfg_get)(cfn_hal_wdt_t *driver, cfn_hal_wdt_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_wdt_cfg_set)(cfn_hal_wdt_t *driver, const cfn_hal_wdt_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_wdt_start)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*cfn_hal_wdt_stop)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*cfn_hal_wdt_feed)(cfn_hal_wdt_t *driver);
};

/**
 * @brief Generated driver structure for wdt.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_wdt_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_wdt_config_t *config;
 *     const cfn_hal_wdt_api_t    *api;
 *     const cfn_hal_wdt_phy_t    *phy;
 *     cfn_hal_wdt_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_wdt_s cfn_hal_wdt_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(wdt, cfn_hal_wdt_config_t, cfn_hal_wdt_api_t, cfn_hal_wdt_phy_t, cfn_hal_wdt_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief cfn_hal_wdt_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_wdt_register_cb(cfn_hal_wdt_t *driver, cfn_hal_wdt_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_wdt_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_set_cb_arg(cfn_hal_wdt_t *driver, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief cfn_hal_wdt_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_check_error(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_check_error, driver, error);
    return error;
}

/**
 * @brief cfn_hal_wdt_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_init(cfn_hal_wdt_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_wdt_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_cfg_get(cfn_hal_wdt_t *driver, cfn_hal_wdt_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_wdt_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_cfg_set(cfn_hal_wdt_t *driver, const cfn_hal_wdt_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_wdt_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_start(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_start, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_wdt_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_stop(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_stop, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_wdt_feed implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_wdt_feed(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, cfn_hal_wdt_feed, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_WDT_H
