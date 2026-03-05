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
 * @file hal_can.h
 * @brief Controller Area Network (CAN) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_CAN_H
#define CAFFEINE_HAL_HAL_CAN_H

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
    HAL_CAN_INTERRUPT_TX,
    HAL_CAN_INTERRUPT_RX,
    HAL_CAN_INTERRUPT_ERROR
} hal_can_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    uint32_t id;
    bool     is_extended;
    bool     is_rtr;
    uint8_t  data[8];
    uint8_t  length;
} hal_can_msg_t;

typedef struct
{
    void      *port;
    hal_gpio_t tx_pin;
    hal_gpio_t rx_pin;
    void      *user_arg;
} hal_can_phy_t;

typedef struct
{
    uint32_t baudrate;
    bool     auto_retransmit;
    void    *custom;
} hal_can_config_t;

typedef struct hal_can_s hal_can_t;

typedef void (*hal_can_callback_t) (hal_can_t *driver, hal_can_interrupts_t interrupt, hal_can_msg_t *msg,
                                    void *user_arg);

typedef struct hal_can_api_s hal_can_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_can_api_s
{
    hal_error_code_t (*hal_can_init) (hal_can_t *driver);
    hal_error_code_t (*hal_can_deinit) (hal_can_t *driver);
    hal_error_code_t (*hal_can_transmit) (hal_can_t *driver, const hal_can_msg_t *msg, uint32_t timeout);
    hal_error_code_t (*hal_can_receive) (hal_can_t *driver, hal_can_msg_t *msg, uint32_t timeout);
    hal_error_code_t (*hal_can_add_filter) (hal_can_t *driver, uint32_t filter_id, uint32_t filter_mask);
    hal_error_code_t (*hal_can_register_cb) (hal_can_t *driver, hal_can_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_can_cfg_irq_enable) (hal_can_t *driver, hal_can_interrupts_t irq);
    hal_error_code_t (*hal_can_cfg_irq_disable) (hal_can_t *driver, hal_can_interrupts_t irq);
};

HAL_CREATE_DRIVER_TYPE (can, hal_can_config_t, hal_can_api_t, hal_can_phy_t, hal_can_callback_t);

/* Functions inline  ---------------------------------------------*/

/**
 * @brief hal_can_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_init (hal_can_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_CAN;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_CAN, hal_can_init, driver, error);
    return error;
}

/**
 * @brief hal_can_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_deinit (hal_can_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_CAN, hal_can_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_can_transmit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param msg Pointer to the message to transmit.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_transmit (hal_can_t *driver, const hal_can_msg_t *msg, uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, timeout);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CAN, hal_can_transmit, driver, error, msg, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_can_receive implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param msg Pointer to the message structure to populate.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_receive (hal_can_t *driver, hal_can_msg_t *msg, uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, timeout);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CAN, hal_can_receive, driver, error, msg, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_can_add_filter implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param filter_id ID to filter for.
 * @param filter_mask Mask to apply to the filter ID.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_add_filter (hal_can_t *driver, uint32_t filter_id, uint32_t filter_mask)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CAN, hal_can_add_filter, driver, error, filter_id, filter_mask);
    return error;
}

/**
 * @brief hal_can_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_register_cb (hal_can_t *driver, hal_can_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CAN, hal_can_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_can_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_cfg_irq_enable (hal_can_t *driver, hal_can_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CAN, hal_can_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_can_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_can_cfg_irq_disable (hal_can_t *driver, hal_can_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_CAN, hal_can_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CAN_H
