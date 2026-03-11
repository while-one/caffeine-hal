/**GPIOx
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
 * @file cfn_hal_gpio.h
 * @brief General Purpose Input/Output (GPIO) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_GPIO_H
#define CAFFEINE_HAL_HAL_GPIO_H

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
    CFN_HAL_GPIO_INTERRUPT_INPUT,
    CFN_HAL_GPIO_INTERRUPT_OUTPUT,
    CFN_HAL_GPIO_INTERRUPT_ERROR,
} cfn_hal_gpio_interrupts_t;

typedef enum
{
    CFN_HAL_GPIO_STATE_LOW = 0,
    CFN_HAL_GPIO_STATE_HIGH
} cfn_hal_gpio_state_t;

typedef enum
{
    CFN_HAL_GPIO_CFG_MODE_ANALOG,
    CFN_HAL_GPIO_CFG_MODE_INPUT,
    CFN_HAL_GPIO_CFG_MODE_OUTPUT_PP,
    CFN_HAL_GPIO_CFG_MODE_OUTPUT_OD,
    CFN_HAL_GPIO_CFG_MODE_ALTERNATE_PP,
    CFN_HAL_GPIO_CFG_MODE_ALTERNATE_OD,
    CFN_HAL_GPIO_CFG_MODE_IT_FALLING,
    CFN_HAL_GPIO_CFG_MODE_IT_RISING,
    CFN_HAL_GPIO_CFG_MODE_IT_BOTH
} cfn_hal_gpio_cfg_mode_t;

typedef enum
{
    CFN_HAL_GPIO_CFG_PULL_NOPULL,
    CFN_HAL_GPIO_CFG_PULL_PULLUP,
    CFN_HAL_GPIO_CFG_PULL_PULLDOWN
} cfn_hal_gpio_cfg_pull_t;

typedef enum
{
    CFN_HAL_GPIO_CFG_SPEED_LOW,      /**< GPIO low speed         */
    CFN_HAL_GPIO_CFG_SPEED_MEDIUM,   /**< GPIO medium speed      */
    CFN_HAL_GPIO_CFG_SPEED_HIGH,     /**< GPIO high speed        */
    CFN_HAL_GPIO_CFG_SPEED_VERY_HIGH /**< GPIO very high speed   */
} cfn_hal_gpio_cfg_speed_t;

typedef enum
{
    CFN_HAL_GPIO_CFG_STRENGTH_LOW,    /**< GPIO low strength */
    CFN_HAL_GPIO_CFG_STRENGTH_MEDIUM, /**< GPIO medium strength */
    CFN_HAL_GPIO_CFG_STRENGTH_HIGH,   /**< GPIO high strength */
} cfn_hal_gpio_cfg_strength_t;

typedef struct
{
    void *pin;
    void *port;
    void *user_arg;
} cfn_hal_gpio_phy_t;

typedef struct
{
    cfn_hal_gpio_cfg_mode_t     mode;
    cfn_hal_gpio_cfg_pull_t     pull;
    cfn_hal_gpio_cfg_speed_t    speed;
    cfn_hal_gpio_cfg_strength_t strength;
    cfn_hal_gpio_state_t        default_state;
    void                       *alternate;
    void                       *custom;
} cfn_hal_gpio_config_t;

typedef struct cfn_hal_gpio_s cfn_hal_gpio_t;

typedef void (*cfn_hal_gpio_callback_t)(cfn_hal_gpio_t *driver, cfn_hal_gpio_interrupts_t interrupt, void *user_arg);

typedef struct cfn_hal_gpio_api_s cfn_hal_gpio_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_gpio_api_s
{
    cfn_hal_error_code_t (*cfn_hal_gpio_register_cb)(
        cfn_hal_gpio_t *driver, cfn_hal_gpio_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_gpio_set_cb_arg)(cfn_hal_gpio_t *driver, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_gpio_read)(cfn_hal_gpio_t *driver, cfn_hal_gpio_state_t *state);
    cfn_hal_error_code_t (*cfn_hal_gpio_write)(cfn_hal_gpio_t *driver, cfn_hal_gpio_state_t state);
    cfn_hal_error_code_t (*cfn_hal_gpio_toggle)(cfn_hal_gpio_t *driver);
    cfn_hal_error_code_t (*cfn_hal_gpio_check_error)(cfn_hal_gpio_t *driver);
    cfn_hal_error_code_t (*cfn_hal_gpio_init)(cfn_hal_gpio_t *driver);
    cfn_hal_error_code_t (*cfn_hal_gpio_deinit)(cfn_hal_gpio_t *driver);
    cfn_hal_error_code_t (*cfn_hal_gpio_cfg_get)(cfn_hal_gpio_t *driver, cfn_hal_gpio_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_gpio_cfg_set)(cfn_hal_gpio_t *driver, const cfn_hal_gpio_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_gpio_cfg_irq_enable)(cfn_hal_gpio_t *driver, cfn_hal_gpio_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_gpio_cfg_irq_disable)(cfn_hal_gpio_t *driver, cfn_hal_gpio_interrupts_t irq);
};

/**
 * @brief Generated driver structure for gpio.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_gpio_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_gpio_config_t *config;
 *     const cfn_hal_gpio_api_t    *api;
 *     const cfn_hal_gpio_phy_t    *phy;
 *     cfn_hal_gpio_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_gpio_s cfn_hal_gpio_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(
    gpio, cfn_hal_gpio_config_t, cfn_hal_gpio_api_t, cfn_hal_gpio_phy_t, cfn_hal_gpio_callback_t);

/* Types Structs ----------------------------------------------------*/
/* Functions prototypes ---------------------------------------------*/
/**
 * @brief cfn_hal_gpio_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_gpio_register_cb(cfn_hal_gpio_t *driver, cfn_hal_gpio_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_gpio_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_set_cb_arg(cfn_hal_gpio_t *driver, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief cfn_hal_gpio_read implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param state Pointer to store the read state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_read(cfn_hal_gpio_t *driver, cfn_hal_gpio_state_t *state)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_read, driver, error, state);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_gpio_write implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param state The state value to write.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_write(cfn_hal_gpio_t *driver, cfn_hal_gpio_state_t state)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_write, driver, error, state);
    CFN_HAL_UNLOCK(driver);
    return error;
}

static inline cfn_hal_error_code_t cfn_hal_gpio_toggle(cfn_hal_gpio_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_toggle, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}
/**
 * @brief cfn_hal_gpio_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_check_error(cfn_hal_gpio_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_check_error, driver, error);
    return error;
}

/**
 * @brief cfn_hal_gpio_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_init(cfn_hal_gpio_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_gpio_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_deinit(cfn_hal_gpio_t *driver)

{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;

    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_deinit, driver, error);

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
/**
 * @brief cfn_hal_gpio_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_cfg_irq_enable(cfn_hal_gpio_t *driver, cfn_hal_gpio_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_gpio_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_gpio_cfg_irq_disable(cfn_hal_gpio_t *driver, cfn_hal_gpio_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, cfn_hal_gpio_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_GPIO_H
