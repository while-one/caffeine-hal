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
 * @file cfn_hal_eth.h
 * @brief Ethernet / MAC HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_ETH_H
#define CAFFEINE_HAL_HAL_ETH_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_base.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief Ethernet nominal event flags.
 */
typedef enum
{
    CFN_HAL_ETH_EVENT_NONE = 0,
    CFN_HAL_ETH_EVENT_RX_COMPLETE = CFN_HAL_BIT(0), /*!< Frame received */
    CFN_HAL_ETH_EVENT_TX_COMPLETE = CFN_HAL_BIT(1), /*!< Frame transmitted */
    CFN_HAL_ETH_EVENT_LINK_UP = CFN_HAL_BIT(2),     /*!< Ethernet link established */
    CFN_HAL_ETH_EVENT_LINK_DOWN = CFN_HAL_BIT(3),   /*!< Ethernet link lost */
} cfn_hal_eth_event_t;

/**
 * @brief Ethernet exception error flags.
 */
typedef enum
{
    CFN_HAL_ETH_ERROR_NONE = 0,
    CFN_HAL_ETH_ERROR_MAC = CFN_HAL_BIT(0),     /*!< MAC layer error (CRC, jabber, etc) */
    CFN_HAL_ETH_ERROR_PHY = CFN_HAL_BIT(1),     /*!< PHY layer or MDIO error */
    CFN_HAL_ETH_ERROR_GENERAL = CFN_HAL_BIT(2), /*!< General hardware error */
} cfn_hal_eth_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Ethernet configuration structure.
 */
typedef struct
{
    uint8_t mac_addr[6]; /*!< Device MAC address */
    void   *user_config; /*!< Vendor-specific MAC/PHY configuration */
} cfn_hal_eth_config_t;

/**
 * @brief Ethernet hardware physical mapping.
 */
typedef struct
{
    void *port;     /*!< Ethernet controller base register address */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_eth_phy_t;

typedef struct cfn_hal_eth_s     cfn_hal_eth_t;
typedef struct cfn_hal_eth_api_s cfn_hal_eth_api_t;

/**
 * @brief Ethernet callback signature.
 * @param driver Pointer to the Ethernet driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_eth_callback_t)(cfn_hal_eth_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief Ethernet Virtual Method Table (VMT).
 */
struct cfn_hal_eth_api_s
{
    cfn_hal_api_base_t base;

    /* Ethernet Specific Extensions can be added here (e.g., transmit/receive frame) */
};

CFN_HAL_CREATE_DRIVER_TYPE(eth, cfn_hal_eth_config_t, cfn_hal_eth_api_t, cfn_hal_eth_phy_t, cfn_hal_eth_callback_t);

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the Ethernet driver.
 * @param driver Pointer to the Ethernet driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_init(cfn_hal_eth_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH);
}

/**
 * @brief Deinitializes the Ethernet driver.
 * @param driver Pointer to the Ethernet driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_deinit(cfn_hal_eth_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH);
}

/**
 * @brief Sets the Ethernet configuration.
 * @param driver Pointer to the Ethernet driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_config_set(cfn_hal_eth_t *driver, const cfn_hal_eth_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, (const void *) config);
}

/**
 * @brief Gets the current Ethernet configuration.
 * @param driver Pointer to the Ethernet driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_config_get(cfn_hal_eth_t *driver, cfn_hal_eth_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for Ethernet events and errors.
 * @param driver Pointer to the Ethernet driver instance.
 * @param CALLBACK The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_eth_callback_register(cfn_hal_eth_t *driver, const cfn_hal_eth_callback_t CALLBACK, void *user_arg)
{
    if (driver)
    {
        driver->cb = CALLBACK;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, (const void *) CALLBACK, user_arg);
}

/**
 * @brief Sets the Ethernet power state.
 * @param driver Pointer to the Ethernet driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_power_state_set(cfn_hal_eth_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, state);
}

/**
 * @brief Enables one or more Ethernet nominal events.
 * @param driver Pointer to the Ethernet driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_event_enable(cfn_hal_eth_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, event_mask);
}

/**
 * @brief Disables one or more Ethernet nominal events.
 * @param driver Pointer to the Ethernet driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_event_disable(cfn_hal_eth_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, event_mask);
}

/**
 * @brief Retrieves the current Ethernet nominal event status.
 * @param driver Pointer to the Ethernet driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_event_get(cfn_hal_eth_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, event_mask);
}

/**
 * @brief Enables one or more Ethernet exception errors.
 * @param driver Pointer to the Ethernet driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_error_enable(cfn_hal_eth_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, error_mask);
}

/**
 * @brief Disables one or more Ethernet exception errors.
 * @param driver Pointer to the Ethernet driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_error_disable(cfn_hal_eth_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, error_mask);
}

/**
 * @brief Retrieves the current Ethernet exception error status.
 * @param driver Pointer to the Ethernet driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_eth_error_get(cfn_hal_eth_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_ETH, error_mask);
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_ETH_H
