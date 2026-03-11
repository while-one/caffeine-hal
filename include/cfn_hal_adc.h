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
 * @file cfn_hal_adc.h
 * @brief Analog-to-Digital Converter (ADC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_ADC_H
#define CAFFEINE_HAL_HAL_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_gpio.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_ADC_INTERRUPT_EOC,
    CFN_HAL_ADC_INTERRUPT_OVR,
    CFN_HAL_ADC_INTERRUPT_ERROR
} cfn_hal_adc_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void          *port;
    cfn_hal_gpio_t pin;
    void          *user_arg;
} cfn_hal_adc_phy_t;

typedef struct
{
    uint32_t resolution;
    uint32_t alignment;
    void    *custom;
} cfn_hal_adc_config_t;

typedef struct cfn_hal_adc_s cfn_hal_adc_t;

typedef void (*cfn_hal_adc_callback_t)(
    cfn_hal_adc_t *driver, cfn_hal_adc_interrupts_t interrupt, uint32_t value, void *user_arg);

typedef struct cfn_hal_adc_api_s cfn_hal_adc_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_adc_api_s
{
    cfn_hal_error_code_t (*cfn_hal_adc_init)(cfn_hal_adc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_adc_deinit)(cfn_hal_adc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_adc_read)(cfn_hal_adc_t *driver, uint32_t *value, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_adc_start)(cfn_hal_adc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_adc_stop)(cfn_hal_adc_t *driver);
    cfn_hal_error_code_t (*cfn_hal_adc_register_cb)(cfn_hal_adc_t *driver, cfn_hal_adc_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_adc_cfg_irq_enable)(cfn_hal_adc_t *driver, cfn_hal_adc_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_adc_cfg_irq_disable)(cfn_hal_adc_t *driver, cfn_hal_adc_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_adc_read_dma)(cfn_hal_adc_t *driver, uint32_t *data, size_t nbr_of_samples);
};

/**
 * @brief Generated driver structure for adc.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_adc_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_adc_config_t *config;
 *     const cfn_hal_adc_api_t    *api;
 *     const cfn_hal_adc_phy_t    *phy;
 *     cfn_hal_adc_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_adc_s cfn_hal_adc_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(adc, cfn_hal_adc_config_t, cfn_hal_adc_api_t, cfn_hal_adc_phy_t, cfn_hal_adc_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief cfn_hal_adc_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_init(cfn_hal_adc_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_adc_read implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param value Pointer to store the read state, or the state value to
 * write.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_read(cfn_hal_adc_t *driver, uint32_t *value, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_read, driver, error, value, timeout);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_adc_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_start(cfn_hal_adc_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_start, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_adc_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_stop(cfn_hal_adc_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_stop, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_adc_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_adc_register_cb(cfn_hal_adc_t *driver, cfn_hal_adc_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_adc_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_cfg_irq_enable(cfn_hal_adc_t *driver, cfn_hal_adc_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_adc_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_cfg_irq_disable(cfn_hal_adc_t *driver, cfn_hal_adc_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_adc_read_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the buffer where samples will be stored.
 * @param nbr_of_samples Number of samples to read.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_adc_read_dma(cfn_hal_adc_t *driver, uint32_t *data, size_t nbr_of_samples)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_ADC, cfn_hal_adc_read_dma, driver, error, data, nbr_of_samples);
    CFN_HAL_UNLOCK(driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_ADC_H
