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
 * @file hal_dac.h
 * @brief Digital-to-Analog Converter (DAC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_DAC_H
#define CAFFEINE_HAL_HAL_DAC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
#include "hal_gpio.h"

/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_DAC_INTERRUPT_UNDERRUN,
    HAL_DAC_INTERRUPT_DMA_HALF,
    HAL_DAC_INTERRUPT_DMA_COMPLETE
} hal_dac_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void      *port;
    uint32_t   channel;
    hal_gpio_t pin;
    void      *user_arg;
} hal_dac_phy_t;

typedef struct
{
    uint32_t resolution;
    uint32_t alignment;
    void    *custom;
} hal_dac_config_t;

typedef struct hal_dac_s hal_dac_t;

typedef void (*hal_dac_callback_t) (hal_dac_t *driver, hal_dac_interrupts_t interrupt, void *user_arg);

typedef struct hal_dac_api_s hal_dac_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_dac_api_s
{
    hal_error_code_t (*hal_dac_init) (hal_dac_t *driver);
    hal_error_code_t (*hal_dac_deinit) (hal_dac_t *driver);
    hal_error_code_t (*hal_dac_set_value) (hal_dac_t *driver, uint32_t value);
    hal_error_code_t (*hal_dac_start) (hal_dac_t *driver);
    hal_error_code_t (*hal_dac_stop) (hal_dac_t *driver);
    hal_error_code_t (*hal_dac_register_cb) (hal_dac_t *driver, hal_dac_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_dac_cfg_irq_enable) (hal_dac_t *driver, hal_dac_interrupts_t irq);
    hal_error_code_t (*hal_dac_cfg_irq_disable) (hal_dac_t *driver, hal_dac_interrupts_t irq);
    hal_error_code_t (*hal_dac_write_dma) (hal_dac_t *driver, const uint32_t *data, size_t nbr_of_samples);
};

HAL_CREATE_DRIVER_TYPE (dac, hal_dac_config_t, hal_dac_api_t, hal_dac_phy_t, hal_dac_callback_t);

/* Functions inline  ---------------------------------------------*/

/**
 * @brief hal_dac_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_init (hal_dac_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_DAC;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DAC, hal_dac_init, driver, error);
    return error;
}

/**
 * @brief hal_dac_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_deinit (hal_dac_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DAC, hal_dac_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_dac_set_value implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param value The digital value to convert.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_set_value (hal_dac_t *driver, uint32_t value)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DAC, hal_dac_set_value, driver, error, value);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_dac_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_start (hal_dac_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DAC, hal_dac_start, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_dac_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_stop (hal_dac_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DAC, hal_dac_stop, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_dac_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_register_cb (hal_dac_t *driver, hal_dac_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DAC, hal_dac_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_dac_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_cfg_irq_enable (hal_dac_t *driver, hal_dac_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DAC, hal_dac_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_dac_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_cfg_irq_disable (hal_dac_t *driver, hal_dac_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DAC, hal_dac_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief hal_dac_write_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the buffer containing samples to write.
 * @param nbr_of_samples Number of samples to write.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dac_write_dma (hal_dac_t *driver, const uint32_t *data, size_t nbr_of_samples)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DAC, hal_dac_write_dma, driver, error, data, nbr_of_samples);
    HAL_UNLOCK (driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_DAC_H
