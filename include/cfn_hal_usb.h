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
 * @file cfn_hal_usb.h
 * @brief USB Device HAL API (PCD Layer).
 * This HAL focuses strictly on the Peripheral Controller Device (PCD) hardware.
 * High-level USB stacks utilize this interface to manage endpoints and transfers.
 */

#ifndef CAFFEINE_HAL_HAL_USB_H
#define CAFFEINE_HAL_HAL_USB_H

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
 * @brief USB nominal event flags.
 * These events drive the state machine of the upper USB stack.
 */
typedef enum
{
    CFN_HAL_USB_EVENT_NONE        = 0,
    CFN_HAL_USB_EVENT_SUSPEND     = CFN_HAL_BIT(0), /*!< Bus entered low power state */
    CFN_HAL_USB_EVENT_RESUME      = CFN_HAL_BIT(1), /*!< Bus activity resumed */
    CFN_HAL_USB_EVENT_RESET       = CFN_HAL_BIT(2), /*!< USB bus reset detected */
    CFN_HAL_USB_EVENT_SETUP_READY = CFN_HAL_BIT(3), /*!< Setup packet received on EP0 */
    CFN_HAL_USB_EVENT_EP_DATA_OUT = CFN_HAL_BIT(4), /*!< Data received on endpoint (Host -> Device) */
    CFN_HAL_USB_EVENT_EP_DATA_IN  = CFN_HAL_BIT(5), /*!< Data transmitted on endpoint (Device -> Host) */
} cfn_hal_usb_event_t;

/**
 * @brief USB exception error flags.
 */
typedef enum
{
    CFN_HAL_USB_ERROR_NONE    = 0,
    CFN_HAL_USB_ERROR_ISO_OUT = CFN_HAL_BIT(0), /*!< Isochronous OUT data lost */
    CFN_HAL_USB_ERROR_ISO_IN  = CFN_HAL_BIT(1), /*!< Isochronous IN data lost */
    CFN_HAL_USB_ERROR_GENERAL = CFN_HAL_BIT(2), /*!< General hardware error */
} cfn_hal_usb_error_t;

/**
 * @brief USB endpoint types.
 */
typedef enum
{
    CFN_HAL_USB_EP_TYPE_CTRL, /*!< Control endpoint */
    CFN_HAL_USB_EP_TYPE_ISOC, /*!< Isochronous endpoint */
    CFN_HAL_USB_EP_TYPE_BULK, /*!< Bulk endpoint */
    CFN_HAL_USB_EP_TYPE_INTR, /*!< Interrupt endpoint */
} cfn_hal_usb_ep_type_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief USB configuration structure.
 */
typedef struct
{
    void *user_config; /*!< Vendor-specific USB hardware configuration */
} cfn_hal_usb_config_t;

/**
 * @brief USB hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance (e.g. USB_OTG_FS) */
    cfn_hal_gpio_pin_handle_t *dp;       /*!< Data+ pin mapping */
    cfn_hal_gpio_pin_handle_t *dm;       /*!< Data- pin mapping */
    cfn_hal_gpio_pin_handle_t *id;       /*!< ID pin mapping (OTG) */
    cfn_hal_gpio_pin_handle_t *vbus;     /*!< VBUS sensing pin mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_usb_phy_t;

typedef struct cfn_hal_usb_s     cfn_hal_usb_t;
typedef struct cfn_hal_usb_api_s cfn_hal_usb_api_t;

/**
 * @brief USB callback signature.
 * @param driver Pointer to the USB driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param ep_addr Endpoint address associated with the event (if applicable).
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_usb_callback_t)(
    cfn_hal_usb_t *driver, uint32_t event_mask, uint32_t error_mask, uint8_t ep_addr, void *user_arg);

/**
 * @brief USB Virtual Method Table (VMT).
 * Focuses exclusively on Peripheral Controller Device (PCD) functions.
 */
struct cfn_hal_usb_api_s
{
    cfn_hal_api_base_t base;

    /* USB PCD Extensions */
    cfn_hal_error_code_t (*start)(cfn_hal_usb_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_usb_t *driver);
    cfn_hal_error_code_t (*set_address)(cfn_hal_usb_t *driver, uint8_t address);
    cfn_hal_error_code_t (*ep_open)(cfn_hal_usb_t        *driver,
                                    uint8_t               ep_addr,
                                    cfn_hal_usb_ep_type_t ep_type,
                                    uint16_t              ep_mps);
    cfn_hal_error_code_t (*ep_close)(cfn_hal_usb_t *driver, uint8_t ep_addr);
    cfn_hal_error_code_t (*ep_transmit)(cfn_hal_usb_t *driver, uint8_t ep_addr, const uint8_t *data, size_t length);
    cfn_hal_error_code_t (*ep_receive)(cfn_hal_usb_t *driver, uint8_t ep_addr, uint8_t *buffer, size_t length);
    cfn_hal_error_code_t (*ep_stall)(cfn_hal_usb_t *driver, uint8_t ep_addr, bool stall);

    /* Stack Integration Helpers */
    cfn_hal_error_code_t (*read_setup_packet)(cfn_hal_usb_t *driver, uint8_t *buffer);
    cfn_hal_error_code_t (*get_rx_data_size)(cfn_hal_usb_t *driver, uint8_t ep_addr, size_t *size);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_usb_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(usb, cfn_hal_usb_config_t, cfn_hal_usb_api_t, cfn_hal_usb_phy_t, cfn_hal_usb_callback_t);

#define CFN_HAL_USB_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_USB, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the USB configuration.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_usb_config_validate(const cfn_hal_usb_t        *driver,
                                                               const cfn_hal_usb_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, config);
}

/**
 * @brief Initializes the USB driver.
 * @param driver Pointer to the USB driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_init(cfn_hal_usb_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB);
}

/**
 * @brief Deinitializes the USB driver.
 * @param driver Pointer to the USB driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_deinit(cfn_hal_usb_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB);
}

/**
 * @brief Sets the USB configuration.
 * @param driver Pointer to the USB driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_config_set(cfn_hal_usb_t *driver, const cfn_hal_usb_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, (const void *) config);
}

/**
 * @brief Gets the current USB configuration.
 * @param driver Pointer to the USB driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_config_get(cfn_hal_usb_t *driver, cfn_hal_usb_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for USB events and errors.
 * @param driver Pointer to the USB driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_callback_register(cfn_hal_usb_t               *driver,
                                                                  const cfn_hal_usb_callback_t callback,
                                                                  void                        *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->cb          = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the USB power state.
 * @param driver Pointer to the USB driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_power_state_set(cfn_hal_usb_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, state);
}

/**
 * @brief Enables one or more USB nominal events.
 * @param driver Pointer to the USB driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_event_enable(cfn_hal_usb_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, event_mask);
}

/**
 * @brief Disables one or more USB nominal events.
 * @param driver Pointer to the USB driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_event_disable(cfn_hal_usb_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, event_mask);
}

/**
 * @brief Retrieves the current USB nominal event status.
 * @param driver Pointer to the USB driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_event_get(cfn_hal_usb_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, event_mask);
}

/**
 * @brief Enables one or more USB exception errors.
 * @param driver Pointer to the USB driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_error_enable(cfn_hal_usb_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, error_mask);
}

/**
 * @brief Disables one or more USB exception errors.
 * @param driver Pointer to the USB driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_error_disable(cfn_hal_usb_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, error_mask);
}

/**
 * @brief Retrieves the current USB exception error status.
 * @param driver Pointer to the USB driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_error_get(cfn_hal_usb_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_USB, error_mask);
}

/* USB Specific Functions ------------------------------------------- */

/**
 * @brief Starts the USB device controller (connects pull-up).
 * @param driver Pointer to the USB driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_start(cfn_hal_usb_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_USB, start, driver, error);
    return error;
}

/**
 * @brief Stops the USB device controller (disconnects pull-up).
 * @param driver Pointer to the USB driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_stop(cfn_hal_usb_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_USB, stop, driver, error);
    return error;
}

/**
 * @brief Sets the USB device address.
 * @param driver Pointer to the USB driver instance.
 * @param address Address assigned by the host (0 to 127).
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_set_address(cfn_hal_usb_t *driver, uint8_t address)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, set_address, driver, error, address);
    return error;
}

/**
 * @brief Configures and enables a USB endpoint.
 * @param driver Pointer to the USB driver instance.
 * @param ep_addr Endpoint address (including direction bit).
 * @param ep_type Endpoint transfer type (Bulk, Interrupt, etc.).
 * @param ep_mps Maximum Packet Size for the endpoint.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_ep_open(cfn_hal_usb_t        *driver,
                                                        uint8_t               ep_addr,
                                                        cfn_hal_usb_ep_type_t ep_type,
                                                        uint16_t              ep_mps)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, ep_open, driver, error, ep_addr, ep_type, ep_mps);
    return error;
}

/**
 * @brief Disables and closes a USB endpoint.
 * @param driver Pointer to the USB driver instance.
 * @param ep_addr Endpoint address.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_ep_close(cfn_hal_usb_t *driver, uint8_t ep_addr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, ep_close, driver, error, ep_addr);
    return error;
}

/**
 * @brief Initiates data transmission on an IN endpoint.
 * @param driver Pointer to the USB driver instance.
 * @param ep_addr IN endpoint address.
 * @param data Pointer to the buffer containing data to send.
 * @param length Number of bytes to transmit.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_ep_transmit(cfn_hal_usb_t *driver,
                                                            uint8_t        ep_addr,
                                                            const uint8_t *data,
                                                            size_t         length)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, ep_transmit, driver, error, ep_addr, data, length);
    return error;
}

/**
 * @brief Prepares an OUT endpoint for data reception.
 * @param driver Pointer to the USB driver instance.
 * @param ep_addr OUT endpoint address.
 * @param buffer Pointer to the buffer where received data will be stored.
 * @param length Maximum number of bytes to receive.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_ep_receive(cfn_hal_usb_t *driver,
                                                           uint8_t        ep_addr,
                                                           uint8_t       *buffer,
                                                           size_t         length)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, ep_receive, driver, error, ep_addr, buffer, length);
    return error;
}

/**
 * @brief Controls the stall condition of an endpoint.
 * @param driver Pointer to the USB driver instance.
 * @param ep_addr Endpoint address.
 * @param stall True to set STALL, False to clear.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_ep_stall(cfn_hal_usb_t *driver, uint8_t ep_addr, bool stall)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, ep_stall, driver, error, ep_addr, stall);
    return error;
}

/**
 * @brief Reads the latest received 8-byte setup packet.
 * @param driver Pointer to the USB driver instance.
 * @param buffer Pointer to the 8-byte buffer to store the setup packet.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_read_setup_packet(cfn_hal_usb_t *driver, uint8_t *buffer)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, read_setup_packet, driver, error, buffer);
    return error;
}

/**
 * @brief Retrieves the size of the last received data on an endpoint.
 * @param driver Pointer to the USB driver instance.
 * @param ep_addr Endpoint address.
 * @param size [out] Pointer to store the number of bytes received.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_usb_get_rx_data_size(cfn_hal_usb_t *driver, uint8_t ep_addr, size_t *size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_USB, get_rx_data_size, driver, error, ep_addr, size);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_USB_H
