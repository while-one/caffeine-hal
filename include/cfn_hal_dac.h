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
 * @file cfn_hal_dac.h
 * @brief Digital-to-Analog Converter (DAC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_DAC_H
#define CAFFEINE_HAL_HAL_DAC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_gpio.h"

/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_DAC_INTERRUPT_UNDERRUN,
    CFN_HAL_DAC_INTERRUPT_DMA_HALF,
    CFN_HAL_DAC_INTERRUPT_DMA_COMPLETE
} cfn_hal_dac_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void          *port;
    uint32_t       channel;
    cfn_hal_gpio_t pin;
    void          *user_arg;
} cfn_hal_dac_phy_t;

typedef struct
{
    uint32_t resolution;
    uint32_t alignment;
    void    *custom;
} cfn_hal_dac_config_t;

typedef struct cfn_hal_dac_s cfn_hal_dac_t;

typedef void (*cfn_hal_dac_callback_t)(cfn_hal_dac_t *driver, cfn_hal_dac_interrupts_t interrupt, void *user_arg);

typedef struct cfn_hal_dac_api_s cfn_hal_dac_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_dac_api_s
{
    cfn_hal_error_code_t (*cfn_hal_dac_init)(cfn_hal_dac_t *driver);
    cfn_hal_error_code_t (*cfn_hal_dac_deinit)(cfn_hal_dac_t *driver);
    cfn_hal_error_code_t (*cfn_hal_dac_set_value)(cfn_hal_dac_t *driver, uint32_t value);
    cfn_hal_error_code_t (*cfn_hal_dac_start)(cfn_hal_dac_t *driver);
    cfn_hal_error_code_t (*cfn_hal_dac_stop)(cfn_hal_dac_t *driver);
    cfn_hal_error_code_t (*cfn_hal_dac_register_cb)(cfn_hal_dac_t *driver, cfn_hal_dac_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_dac_cfg_irq_enable)(cfn_hal_dac_t *driver, cfn_hal_dac_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_dac_cfg_irq_disable)(cfn_hal_dac_t *driver, cfn_hal_dac_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_dac_write_dma)(cfn_hal_dac_t *driver, const uint32_t *data, size_t nbr_of_samples);
};

/**
 * @brief Generated driver structure for dac.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_dac_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_dac_config_t *config;
 *     const cfn_hal_dac_api_t    *api;
 *     const cfn_hal_dac_phy_t    *phy;
 *     cfn_hal_dac_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_dac_s cfn_hal_dac_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(dac, cfn_hal_dac_config_t, cfn_hal_dac_api_t, cfn_hal_dac_phy_t, cfn_hal_dac_callback_t);

/* Functions inline  ---------------------------------------------*/

/**
 * @brief cfn_hal_dac_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_dac_init(cfn_hal_dac_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_dac_set_value implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param value The digital value to convert.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_dac_set_value(cfn_hal_dac_t *driver, uint32_t value)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_set_value, driver, error, value);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_dac_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_dac_start(cfn_hal_dac_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_start, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_dac_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_dac_stop(cfn_hal_dac_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_stop, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_dac_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_dac_register_cb(cfn_hal_dac_t *driver, cfn_hal_dac_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_dac_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_dac_cfg_irq_enable(cfn_hal_dac_t *driver, cfn_hal_dac_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_dac_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_dac_cfg_irq_disable(cfn_hal_dac_t *driver, cfn_hal_dac_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_dac_write_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the buffer containing samples to write.
 * @param nbr_of_samples Number of samples to write.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_dac_write_dma(cfn_hal_dac_t *driver, const uint32_t *data, size_t nbr_of_samples)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_DAC, cfn_hal_dac_write_dma, driver, error, data, nbr_of_samples);
    CFN_HAL_UNLOCK(driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_DAC_H
