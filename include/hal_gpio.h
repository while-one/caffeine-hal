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
 * @file hal_gpio.h
 * @brief General Purpose Input/Output (GPIO) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_GPIO_H
#define CAFFEINE_HAL_HAL_GPIO_H

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
    HAL_GPIO_INTERRUPT_INPUT,
    HAL_GPIO_INTERRUPT_OUTPUT,
    HAL_GPIO_INTERRUPT_ERROR,
} hal_gpio_interrupts_t;

typedef enum
{
    HAL_GPIO_STATE_LOW = 0,
    HAL_GPIO_STATE_HIGH
} hal_gpio_state_t;

typedef enum
{
    HAL_GPIO_CFG_MODE_ANALOG,
    HAL_GPIO_CFG_MODE_INPUT,
    HAL_GPIO_CFG_MODE_OUTPUT_PP,
    HAL_GPIO_CFG_MODE_OUTPUT_OD,
    HAL_GPIO_CFG_MODE_ALTERNATE_PP,
    HAL_GPIO_CFG_MODE_ALTERNATE_OD,
    HAL_GPIO_CFG_MODE_IT_FALLING,
    HAL_GPIO_CFG_MODE_IT_RISING,
    HAL_GPIO_CFG_MODE_IT_BOTH
} hal_gpio_cfg_mode_t;

typedef enum
{
    HAL_GPIO_CFG_PULL_NOPULL,
    HAL_GPIO_CFG_PULL_PULLUP,
    HAL_GPIO_CFG_PULL_PULLDOWN
} hal_gpio_cfg_pull_t;

typedef enum
{
    HAL_GPIO_CFG_SPEED_LOW,      /**< GPIO low speed         */
    HAL_GPIO_CFG_SPEED_MEDIUM,   /**< GPIO medium speed      */
    HAL_GPIO_CFG_SPEED_HIGH,     /**< GPIO high speed        */
    HAL_GPIO_CFG_SPEED_VERY_HIGH /**< GPIO very high speed   */
} hal_gpio_cfg_speed_t;

typedef enum
{
    HAL_GPIO_CFG_STRENGTH_LOW,    /**< GPIO low strength */
    HAL_GPIO_CFG_STRENGTH_MEDIUM, /**< GPIO medium strength */
    HAL_GPIO_CFG_STRENGTH_HIGH,   /**< GPIO high strength */
} hal_gpio_cfg_strength_t;

typedef struct
{
    void *pin;
    void *port;
    void *user_arg;
} hal_gpio_phy_t;

typedef struct
{
    hal_gpio_cfg_mode_t     mode;
    hal_gpio_cfg_pull_t     pull;
    hal_gpio_cfg_speed_t    speed;
    hal_gpio_cfg_strength_t strength;
    hal_gpio_state_t        default_state;
    void                   *alternate;
    void                   *custom;
} hal_gpio_config_t;

typedef struct hal_gpio_s hal_gpio_t;

typedef void (*hal_gpio_callback_t) (hal_gpio_t *driver, hal_gpio_interrupts_t interrupt, void *user_arg);

typedef struct hal_gpio_api_s hal_gpio_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_gpio_api_s
{
    hal_error_code_t (*hal_gpio_register_cb) (hal_gpio_t *driver, hal_gpio_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_gpio_set_cb_arg) (hal_gpio_t *driver, void *user_arg);
    hal_error_code_t (*hal_gpio_read) (hal_gpio_t *driver, hal_gpio_state_t *state);
    hal_error_code_t (*hal_gpio_write) (hal_gpio_t *driver, hal_gpio_state_t state);
    hal_error_code_t (*hal_gpio_check_error) (hal_gpio_t *driver);
    hal_error_code_t (*hal_gpio_init) (hal_gpio_t *driver);
    hal_error_code_t (*hal_gpio_deinit) (hal_gpio_t *driver);
    hal_error_code_t (*hal_gpio_cfg_get) (hal_gpio_t *driver, hal_gpio_config_t *config);
    hal_error_code_t (*hal_gpio_cfg_set) (hal_gpio_t *driver, const hal_gpio_config_t *config);
    hal_error_code_t (*hal_gpio_cfg_irq_enable) (hal_gpio_t *driver, hal_gpio_interrupts_t irq);
    hal_error_code_t (*hal_gpio_cfg_irq_disable) (hal_gpio_t *driver, hal_gpio_interrupts_t irq);
};

HAL_CREATE_DRIVER_TYPE (gpio, hal_gpio_config_t, hal_gpio_api_t, hal_gpio_phy_t, hal_gpio_callback_t);

/* Types Structs ----------------------------------------------------*/
/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_gpio_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_register_cb (hal_gpio_t *driver, hal_gpio_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_gpio_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_set_cb_arg (hal_gpio_t *driver, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief hal_gpio_read implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param state Pointer to store the read state, or the state value to
 * write.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_read (hal_gpio_t *driver, hal_gpio_state_t *state)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_read, driver, error, state);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_gpio_write implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param state Pointer to store the read state, or the state value to
 * write.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_write (hal_gpio_t *driver, hal_gpio_state_t state)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_write, driver, error, state);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_gpio_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_check_error (hal_gpio_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_check_error, driver, error);
    return error;
}

/**
 * @brief hal_gpio_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_init (hal_gpio_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_GPIO;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_init, driver, error);
    return error;
}

/**
 * @brief hal_gpio_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_cfg_get (hal_gpio_t *driver, hal_gpio_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_gpio_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_cfg_set (hal_gpio_t *driver, const hal_gpio_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_gpio_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_deinit (hal_gpio_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_gpio_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_cfg_irq_enable (hal_gpio_t *driver, hal_gpio_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_gpio_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_gpio_cfg_irq_disable (hal_gpio_t *driver, hal_gpio_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_GPIO, hal_gpio_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_GPIO_H
