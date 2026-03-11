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
 * @file cfn_hal_clock.h
 * @brief Clock Management HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_CLOCK_H
#define CAFFEINE_HAL_HAL_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"

/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void *user_config;
} cfn_hal_clock_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_clock_phy_t;

typedef struct cfn_hal_clock_s     cfn_hal_clock_t;
typedef struct cfn_hal_clock_api_s cfn_hal_clock_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_clock_api_s
{
    cfn_hal_error_code_t (*cfn_hal_clock_init)(cfn_hal_clock_t *driver);
    cfn_hal_error_code_t (*cfn_hal_clock_deinit)(cfn_hal_clock_t *driver);
    cfn_hal_error_code_t (*cfn_hal_clock_cfg_get)(cfn_hal_clock_t *driver, cfn_hal_clock_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_clock_cfg_set)(cfn_hal_clock_t *driver, const cfn_hal_clock_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_clock_suspend_tick)(cfn_hal_clock_t *driver);
    cfn_hal_error_code_t (*cfn_hal_clock_resume_tick)(cfn_hal_clock_t *driver);
    cfn_hal_error_code_t (*cfn_hal_clock_get_system_freq)(cfn_hal_clock_t *driver, uint32_t *freq_hz);
    cfn_hal_error_code_t (*cfn_hal_clock_get_peripheral_freq)(
        cfn_hal_clock_t *driver, uint32_t peripheral_id, uint32_t *freq_hz);
    cfn_hal_error_code_t (*cfn_hal_clock_enable_gate)(cfn_hal_clock_t *driver, uint32_t peripheral_id);
    cfn_hal_error_code_t (*cfn_hal_clock_disable_gate)(cfn_hal_clock_t *driver, uint32_t peripheral_id);
};

/**
 * @brief Generated driver structure for clock.
 * This macro expands to define `struct cfn_hal_clock_s` and the typedef `cfn_hal_clock_t`.
 */
CFN_HAL_CREATE_DRIVER_TYPE(clock, cfn_hal_clock_config_t, cfn_hal_clock_api_t, cfn_hal_clock_phy_t, void *);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief cfn_hal_clock_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_init(cfn_hal_clock_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_clock_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_cfg_get(cfn_hal_clock_t *driver, cfn_hal_clock_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_clock_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_cfg_set(cfn_hal_clock_t *driver, const cfn_hal_clock_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_clock_suspend_tick implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_suspend_tick(cfn_hal_clock_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_suspend_tick, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_clock_resume_tick implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_resume_tick(cfn_hal_clock_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_resume_tick, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief Get the main system clock frequency.
 * @param driver Pointer to the peripheral driver instance.
 * @param freq_hz Pointer to store the frequency in Hz.
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_get_system_freq(cfn_hal_clock_t *driver, uint32_t *freq_hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_get_system_freq, driver, error, freq_hz);
    return error;
}

/**
 * @brief Get the clock frequency for a specific peripheral domain.
 * @param driver Pointer to the peripheral driver instance.
 * @param peripheral_id ID or FourCC of the peripheral.
 * @param freq_hz Pointer to store the frequency in Hz.
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t
cfn_hal_clock_get_peripheral_freq(cfn_hal_clock_t *driver, uint32_t peripheral_id, uint32_t *freq_hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_get_peripheral_freq, driver, error, peripheral_id, freq_hz);
    return error;
}

/**
 * @brief Enable the clock gate for a specific peripheral.
 * @param driver Pointer to the peripheral driver instance.
 * @param peripheral_id ID or FourCC of the peripheral.
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_enable_gate(cfn_hal_clock_t *driver, uint32_t peripheral_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_enable_gate, driver, error, peripheral_id);
    return error;
}

/**
 * @brief Disable the clock gate for a specific peripheral.
 * @param driver Pointer to the peripheral driver instance.
 * @param peripheral_id ID or FourCC of the peripheral.
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t cfn_hal_clock_disable_gate(cfn_hal_clock_t *driver, uint32_t peripheral_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_CLOCK, cfn_hal_clock_disable_gate, driver, error, peripheral_id);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CLOCK_H
