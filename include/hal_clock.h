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
 * @file hal_clock.h
 * @brief Clock Management HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_CLOCK_H
#define CAFFEINE_HAL_HAL_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"

/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void *user_config;
} hal_clock_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} hal_clock_phy_t;

typedef struct hal_clock_s     hal_clock_t;
typedef struct hal_clock_api_s hal_clock_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_clock_api_s
{
    hal_error_code_t (*hal_clock_init) (hal_clock_t *driver);
    hal_error_code_t (*hal_clock_deinit) (hal_clock_t *driver);
    hal_error_code_t (*hal_clock_cfg_get) (hal_clock_t *driver, hal_clock_config_t *config);
    hal_error_code_t (*hal_clock_cfg_set) (hal_clock_t *driver, const hal_clock_config_t *config);
    hal_error_code_t (*hal_clock_suspend_tick) (hal_clock_t *driver);
    hal_error_code_t (*hal_clock_resume_tick) (hal_clock_t *driver);
    hal_error_code_t (*hal_clock_get_system_freq) (hal_clock_t *driver, uint32_t *freq_hz);
    hal_error_code_t (*hal_clock_get_peripheral_freq) (hal_clock_t *driver, uint32_t peripheral_id, uint32_t *freq_hz);
    hal_error_code_t (*hal_clock_enable_gate) (hal_clock_t *driver, uint32_t peripheral_id);
    hal_error_code_t (*hal_clock_disable_gate) (hal_clock_t *driver, uint32_t peripheral_id);
};

HAL_CREATE_DRIVER_TYPE (clock, hal_clock_config_t, hal_clock_api_t, hal_clock_phy_t, void *);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_clock_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_clock_init (hal_clock_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_CLOCK;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_init, driver, error);
    return error;
}

/**
 * @brief hal_clock_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_clock_deinit (hal_clock_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_clock_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_clock_cfg_get (hal_clock_t *driver, hal_clock_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_clock_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_clock_cfg_set (hal_clock_t *driver, const hal_clock_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_clock_suspend_tick implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_clock_suspend_tick (hal_clock_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_suspend_tick, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_clock_resume_tick implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_clock_resume_tick (hal_clock_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_resume_tick, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief Get the main system clock frequency.
 * @param driver Pointer to the peripheral driver instance.
 * @param freq_hz Pointer to store the frequency in Hz.
 * @return HAL_ERROR_OK on success.
 */
static inline hal_error_code_t hal_clock_get_system_freq (hal_clock_t *driver, uint32_t *freq_hz)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_get_system_freq, driver, error, freq_hz);
    return error;
}

/**
 * @brief Get the clock frequency for a specific peripheral domain.
 * @param driver Pointer to the peripheral driver instance.
 * @param peripheral_id ID or FourCC of the peripheral.
 * @param freq_hz Pointer to store the frequency in Hz.
 * @return HAL_ERROR_OK on success.
 */
static inline hal_error_code_t hal_clock_get_peripheral_freq (hal_clock_t *driver, uint32_t peripheral_id,
                                                              uint32_t *freq_hz)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_get_peripheral_freq, driver, error,
                                  peripheral_id, freq_hz);
    return error;
}

/**
 * @brief Enable the clock gate for a specific peripheral.
 * @param driver Pointer to the peripheral driver instance.
 * @param peripheral_id ID or FourCC of the peripheral.
 * @return HAL_ERROR_OK on success.
 */
static inline hal_error_code_t hal_clock_enable_gate (hal_clock_t *driver, uint32_t peripheral_id)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_enable_gate, driver, error, peripheral_id);
    return error;
}

/**
 * @brief Disable the clock gate for a specific peripheral.
 * @param driver Pointer to the peripheral driver instance.
 * @param peripheral_id ID or FourCC of the peripheral.
 * @return HAL_ERROR_OK on success.
 */
static inline hal_error_code_t hal_clock_disable_gate (hal_clock_t *driver, uint32_t peripheral_id)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CLOCK, hal_clock_disable_gate, driver, error, peripheral_id);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CLOCK_H
