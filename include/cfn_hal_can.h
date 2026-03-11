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
 * @file cfn_hal_can.h
 * @brief CAN (Controller Area Network) Hardware Abstraction Layer.
 */

#ifndef CAFFEINE_HAL_HAL_CAN_H
#define CAFFEINE_HAL_HAL_CAN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"

/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_CAN_INTERRUPT_TX_COMPLETE,
    CFN_HAL_CAN_INTERRUPT_RX_READY,
    CFN_HAL_CAN_INTERRUPT_BUS_OFF,
    CFN_HAL_CAN_INTERRUPT_ERROR,
} cfn_hal_can_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    uint32_t id;
    uint8_t  dlc;
    uint8_t  data[8];
} cfn_hal_can_msg_t;

typedef struct
{
    uint32_t baudrate;
    void    *custom;
} cfn_hal_can_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_can_phy_t;

typedef struct
{
    uint32_t filter_id;
    uint32_t filter_mask;
} cfn_hal_can_filter_t;

typedef struct cfn_hal_can_s cfn_hal_can_t;

typedef void (*cfn_hal_can_callback_t)(
    cfn_hal_can_t *driver, cfn_hal_can_interrupts_t interrupt, cfn_hal_can_msg_t *msg, void *user_arg);

typedef struct cfn_hal_can_api_s cfn_hal_can_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_can_api_s
{
    cfn_hal_error_code_t (*cfn_hal_can_init)(cfn_hal_can_t *driver);
    cfn_hal_error_code_t (*cfn_hal_can_deinit)(cfn_hal_can_t *driver);
    cfn_hal_error_code_t (*cfn_hal_can_transmit)(cfn_hal_can_t *driver, const cfn_hal_can_msg_t *msg, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_can_receive)(cfn_hal_can_t *driver, cfn_hal_can_msg_t *msg, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_can_add_filter)(cfn_hal_can_t *driver, const cfn_hal_can_filter_t *filter);
    cfn_hal_error_code_t (*cfn_hal_can_register_cb)(cfn_hal_can_t *driver, cfn_hal_can_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_can_cfg_irq_enable)(cfn_hal_can_t *driver, cfn_hal_can_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_can_cfg_irq_disable)(cfn_hal_can_t *driver, cfn_hal_can_interrupts_t irq);
};

/**
 * @brief Generated driver structure for can.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_can_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_can_config_t *config;
 *     const cfn_hal_can_api_t    *api;
 *     const cfn_hal_can_phy_t    *phy;
 *     cfn_hal_can_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_can_s cfn_hal_can_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(can, cfn_hal_can_config_t, cfn_hal_can_api_t, cfn_hal_can_phy_t, cfn_hal_can_callback_t);

/* Functions inline  ---------------------------------------------*/

/**
 * @brief cfn_hal_can_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_init(cfn_hal_can_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_can_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_deinit(cfn_hal_can_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_deinit, driver, error);
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
 * @brief cfn_hal_can_transmit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param msg Pointer to the message structure to be transmitted.
 * @param timeout Timeout duration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_can_transmit(cfn_hal_can_t *driver, const cfn_hal_can_msg_t *msg, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_transmit, driver, error, msg, timeout);
    return error;
}

/**
 * @brief cfn_hal_can_receive implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param msg Pointer to the message structure to store received data.
 * @param timeout Timeout duration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_receive(cfn_hal_can_t *driver, cfn_hal_can_msg_t *msg, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_receive, driver, error, msg, timeout);
    return error;
}

/**
 * @brief cfn_hal_can_add_filter implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param filter Pointer to the filter configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_add_filter(cfn_hal_can_t *driver, const cfn_hal_can_filter_t *filter)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_add_filter, driver, error, filter);
    return error;
}

/**
 * @brief cfn_hal_can_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_can_register_cb(cfn_hal_can_t *driver, cfn_hal_can_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_can_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_cfg_irq_enable(cfn_hal_can_t *driver, cfn_hal_can_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_can_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_cfg_irq_disable(cfn_hal_can_t *driver, cfn_hal_can_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, cfn_hal_can_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CAN_H
