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
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_gpio.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief CAN nominal event flags.
 */
typedef enum
{
    CFN_HAL_CAN_EVENT_NONE = 0,
    CFN_HAL_CAN_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Transmission complete */
    CFN_HAL_CAN_EVENT_RX_READY = CFN_HAL_BIT(1),    /*!< Message received and ready */
} cfn_hal_can_event_t;

/**
 * @brief CAN exception error flags.
 */
typedef enum
{
    CFN_HAL_CAN_ERROR_NONE = 0,
    CFN_HAL_CAN_ERROR_BUS_OFF = CFN_HAL_BIT(0), /*!< CAN bus-off state detected */
    CFN_HAL_CAN_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General CAN hardware error */
} cfn_hal_can_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief CAN message structure.
 */
typedef struct
{
    uint32_t id;      /*!< Standard or Extended identifier */
    uint8_t  dlc;     /*!< Data Length Code */
    uint8_t  data[8]; /*!< Message payload */
} cfn_hal_can_msg_t;

/**
 * @brief CAN configuration structure.
 */
typedef struct
{
    uint32_t baudrate; /*!< Target baudrate in bps */
    void    *custom;   /*!< Vendor-specific custom configuration */
} cfn_hal_can_config_t;

/**
 * @brief CAN hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    cfn_hal_gpio_pin_driver_t *tx;       /*!< TX pin driver mapping */
    cfn_hal_gpio_pin_driver_t *rx;       /*!< RX pin driver mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_can_phy_t;

/**
 * @brief CAN filter configuration.
 */
typedef struct
{
    uint32_t filter_id;   /*!< Filter identifier */
    uint32_t filter_mask; /*!< Filter mask */
} cfn_hal_can_filter_t;

typedef struct cfn_hal_can_s     cfn_hal_can_t;
typedef struct cfn_hal_can_api_s cfn_hal_can_api_t;

/**
 * @brief CAN callback signature.
 * @param driver Pointer to the CAN driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param msg Pointer to the received message (only for RX_READY event).
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_can_callback_t)(
    cfn_hal_can_t *driver, uint32_t event_mask, uint32_t error_mask, cfn_hal_can_msg_t *msg, void *user_arg);

/**
 * @brief CAN Virtual Method Table (VMT).
 */
struct cfn_hal_can_api_s
{
    cfn_hal_api_base_t base;

    /* CAN Specific Extensions */
    cfn_hal_error_code_t (*transmit)(cfn_hal_can_t *driver, const cfn_hal_can_msg_t *msg, uint32_t timeout);
    cfn_hal_error_code_t (*receive)(cfn_hal_can_t *driver, cfn_hal_can_msg_t *msg, uint32_t timeout);
    cfn_hal_error_code_t (*add_filter)(cfn_hal_can_t *driver, const cfn_hal_can_filter_t *filter);
};

CFN_HAL_CREATE_DRIVER_TYPE(can, cfn_hal_can_config_t, cfn_hal_can_api_t, cfn_hal_can_phy_t, cfn_hal_can_callback_t);

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the CAN driver.
 * @param driver Pointer to the CAN driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_init(cfn_hal_can_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN);
}

/**
 * @brief Deinitializes the CAN driver.
 * @param driver Pointer to the CAN driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_deinit(cfn_hal_can_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN);
}

/**
 * @brief Sets the CAN configuration.
 * @param driver Pointer to the CAN driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_config_set(cfn_hal_can_t *driver, const cfn_hal_can_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, (const void *) config);
}

/**
 * @brief Gets the current CAN configuration.
 * @param driver Pointer to the CAN driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_config_get(cfn_hal_can_t *driver, cfn_hal_can_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for CAN events and errors.
 * @param driver Pointer to the CAN driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_can_callback_register(cfn_hal_can_t *driver, const cfn_hal_can_callback_t callback, void *user_arg)
{
    if (driver)
    {
        driver->cb = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the CAN power state.
 * @param driver Pointer to the CAN driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_power_state_set(cfn_hal_can_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, state);
}

/**
 * @brief Enables one or more CAN nominal events.
 * @param driver Pointer to the CAN driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_event_enable(cfn_hal_can_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, event_mask);
}

/**
 * @brief Disables one or more CAN nominal events.
 * @param driver Pointer to the CAN driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_event_disable(cfn_hal_can_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, event_mask);
}

/**
 * @brief Retrieves the current CAN nominal event status.
 * @param driver Pointer to the CAN driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_event_get(cfn_hal_can_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, event_mask);
}

/**
 * @brief Enables one or more CAN exception errors.
 * @param driver Pointer to the CAN driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_error_enable(cfn_hal_can_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, error_mask);
}

/**
 * @brief Disables one or more CAN exception errors.
 * @param driver Pointer to the CAN driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_error_disable(cfn_hal_can_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, error_mask);
}

/**
 * @brief Retrieves the current CAN exception error status.
 * @param driver Pointer to the CAN driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_error_get(cfn_hal_can_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CAN, error_mask);
}

/* CAN Specific Functions ------------------------------------------- */

/**
 * @brief Transmits a CAN message (blocking).
 * @param driver Pointer to the CAN driver instance.
 * @param msg Pointer to the message structure to be transmitted.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_can_transmit(cfn_hal_can_t *driver, const cfn_hal_can_msg_t *msg, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, transmit, driver, error, msg, timeout);
    return error;
}

/**
 * @brief Receives a CAN message (blocking).
 * @param driver Pointer to the CAN driver instance.
 * @param msg [out] Pointer to the message structure to store received data.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_receive(cfn_hal_can_t *driver, cfn_hal_can_msg_t *msg, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, receive, driver, error, msg, timeout);
    return error;
}

/**
 * @brief Adds a message filter to the CAN peripheral.
 * @param driver Pointer to the CAN driver instance.
 * @param filter Pointer to the filter configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_can_add_filter(cfn_hal_can_t *driver, const cfn_hal_can_filter_t *filter)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CAN, add_filter, driver, error, filter);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CAN_H
