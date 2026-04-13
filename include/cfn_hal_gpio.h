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
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_types.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief GPIO nominal event flags.
 */
typedef enum
{
    CFN_HAL_GPIO_EVENT_NONE    = 0,
    CFN_HAL_GPIO_EVENT_RISING  = CFN_HAL_BIT(0), /*!< Rising edge detected */
    CFN_HAL_GPIO_EVENT_FALLING = CFN_HAL_BIT(1), /*!< Falling edge detected */
} cfn_hal_gpio_event_t;

/**
 * @brief GPIO exception error flags.
 */
typedef enum
{
    CFN_HAL_GPIO_ERROR_NONE    = 0,
    CFN_HAL_GPIO_ERROR_GENERAL = CFN_HAL_BIT(0), /*!< General GPIO hardware error */
} cfn_hal_gpio_error_t;

typedef enum
{
    CFN_HAL_GPIO_STATE_LOW = 0,
    CFN_HAL_GPIO_STATE_HIGH,

    CFN_HAL_GPIO_STATE_MAX
} cfn_hal_gpio_state_t;

typedef enum
{
    CFN_HAL_GPIO_CONFIG_MODE_ANALOG,
    CFN_HAL_GPIO_CONFIG_MODE_INPUT,
    CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP,
    CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD,
    CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_PP,
    CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_OD,

    CFN_HAL_GPIO_CONFIG_MODE_MAX
} cfn_hal_gpio_config_mode_t;

typedef enum
{
    CFN_HAL_GPIO_CONFIG_PULL_NOPULL,
    CFN_HAL_GPIO_CONFIG_PULL_PULLUP,
    CFN_HAL_GPIO_CONFIG_PULL_PULLDOWN,

    CFN_HAL_GPIO_CONFIG_PULL_MAX
} cfn_hal_gpio_config_pull_t;

typedef enum
{
    CFN_HAL_GPIO_CONFIG_SPEED_LOW,       /**< GPIO low speed         */
    CFN_HAL_GPIO_CONFIG_SPEED_MEDIUM,    /**< GPIO medium speed      */
    CFN_HAL_GPIO_CONFIG_SPEED_HIGH,      /**< GPIO high speed        */
    CFN_HAL_GPIO_CONFIG_SPEED_VERY_HIGH, /**< GPIO very high speed   */

    CFN_HAL_GPIO_CONFIG_SPEED_MAX
} cfn_hal_gpio_config_speed_t;

typedef enum
{
    CFN_HAL_GPIO_CONFIG_STRENGTH_LOW,    /**< GPIO low strength */
    CFN_HAL_GPIO_CONFIG_STRENGTH_MEDIUM, /**< GPIO medium strength */
    CFN_HAL_GPIO_CONFIG_STRENGTH_HIGH,   /**< GPIO high strength */

    CFN_HAL_GPIO_CONFIG_STRENGTH_MAX
} cfn_hal_gpio_config_strength_t;

typedef uint32_t cfn_hal_gpio_pin_t;

#define CFN_HAL_GPIO_PIN_0  CFN_HAL_BIT(0)
#define CFN_HAL_GPIO_PIN_1  CFN_HAL_BIT(1)
#define CFN_HAL_GPIO_PIN_2  CFN_HAL_BIT(2)
#define CFN_HAL_GPIO_PIN_3  CFN_HAL_BIT(3)
#define CFN_HAL_GPIO_PIN_4  CFN_HAL_BIT(4)
#define CFN_HAL_GPIO_PIN_5  CFN_HAL_BIT(5)
#define CFN_HAL_GPIO_PIN_6  CFN_HAL_BIT(6)
#define CFN_HAL_GPIO_PIN_7  CFN_HAL_BIT(7)
#define CFN_HAL_GPIO_PIN_8  CFN_HAL_BIT(8)
#define CFN_HAL_GPIO_PIN_9  CFN_HAL_BIT(9)
#define CFN_HAL_GPIO_PIN_10 CFN_HAL_BIT(10)
#define CFN_HAL_GPIO_PIN_11 CFN_HAL_BIT(11)
#define CFN_HAL_GPIO_PIN_12 CFN_HAL_BIT(12)
#define CFN_HAL_GPIO_PIN_13 CFN_HAL_BIT(13)
#define CFN_HAL_GPIO_PIN_14 CFN_HAL_BIT(14)
#define CFN_HAL_GPIO_PIN_15 CFN_HAL_BIT(15)
#define CFN_HAL_GPIO_PIN_16 CFN_HAL_BIT(16)
#define CFN_HAL_GPIO_PIN_17 CFN_HAL_BIT(17)
#define CFN_HAL_GPIO_PIN_18 CFN_HAL_BIT(18)
#define CFN_HAL_GPIO_PIN_19 CFN_HAL_BIT(19)
#define CFN_HAL_GPIO_PIN_20 CFN_HAL_BIT(20)
#define CFN_HAL_GPIO_PIN_21 CFN_HAL_BIT(21)
#define CFN_HAL_GPIO_PIN_22 CFN_HAL_BIT(22)
#define CFN_HAL_GPIO_PIN_23 CFN_HAL_BIT(23)
#define CFN_HAL_GPIO_PIN_24 CFN_HAL_BIT(24)
#define CFN_HAL_GPIO_PIN_25 CFN_HAL_BIT(25)
#define CFN_HAL_GPIO_PIN_26 CFN_HAL_BIT(26)
#define CFN_HAL_GPIO_PIN_27 CFN_HAL_BIT(27)
#define CFN_HAL_GPIO_PIN_28 CFN_HAL_BIT(28)
#define CFN_HAL_GPIO_PIN_29 CFN_HAL_BIT(29)
#define CFN_HAL_GPIO_PIN_30 CFN_HAL_BIT(30)
#define CFN_HAL_GPIO_PIN_31 CFN_HAL_BIT(31)

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_gpio_phy_t;

typedef struct
{
    cfn_hal_gpio_pin_t             pin_mask; /*!< Pins on this port to apply this config to */
    cfn_hal_gpio_config_mode_t     mode;
    cfn_hal_gpio_config_pull_t     pull;
    cfn_hal_gpio_config_speed_t    speed;
    cfn_hal_gpio_config_strength_t strength;
    cfn_hal_gpio_state_t           default_state;
    void                          *alternate;
    void                          *custom;
} cfn_hal_gpio_pin_config_t;

typedef struct cfn_hal_gpio_s     cfn_hal_gpio_t;
typedef struct cfn_hal_gpio_api_s cfn_hal_gpio_api_t;

/**
 * @brief Optional API extension for external GPIO expanders (e.g., I2C/SPI chips).
 */
typedef struct
{
    cfn_hal_error_code_t (*get_id)(cfn_hal_gpio_t *port, uint32_t *id_out);
    void (*handle_interrupt)(cfn_hal_gpio_t *port);
} cfn_hal_gpio_extender_api_t;

/**
 * @brief Lightweight handle to a specific GPIO pin.
 * Used by other peripherals to reference physical pins.
 */
typedef struct
{
    cfn_hal_gpio_t    *port; /*!< Pointer to the Port driver (e.g., GPIOA) */
    cfn_hal_gpio_pin_t pin;  /*!< Single pin bitmask (e.g., CFN_HAL_GPIO_PIN_9) */
} cfn_hal_gpio_pin_handle_t;

/**
 * @brief GPIO callback signature.
 * @param port Pointer to the GPIO port driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_gpio_callback_t)(cfn_hal_gpio_t *port, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief GPIO Virtual Method Table (VMT).
 */
struct cfn_hal_gpio_api_s
{
    cfn_hal_api_base_t base; /*!< base.config_set should return NOT_SUPPORTED by the port */

    /* GPIO Specific Extensions */
    cfn_hal_error_code_t (*pin_config)(cfn_hal_gpio_t *port, const cfn_hal_gpio_pin_config_t *pin_cfg);

    cfn_hal_error_code_t (*pin_read)(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t *state);
    cfn_hal_error_code_t (*pin_write)(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t state);
    cfn_hal_error_code_t (*pin_toggle)(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin);

    cfn_hal_error_code_t (*port_read)(cfn_hal_gpio_t *port, uint32_t *port_value);
    cfn_hal_error_code_t (*port_write)(cfn_hal_gpio_t *port, uint32_t pin_mask, uint32_t port_value);

    const cfn_hal_gpio_extender_api_t *extender;
};

CFN_HAL_VMT_CHECK(struct cfn_hal_gpio_api_s);

/* Note: 'void' is used for config_type because the port driver does not have a
 * global config state */
CFN_HAL_CREATE_DRIVER_TYPE(gpio, void, cfn_hal_gpio_api_t, cfn_hal_gpio_phy_t, cfn_hal_gpio_callback_t);

/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void cfn_hal_gpio_populate(cfn_hal_gpio_t           *driver,
                                          uint32_t                  peripheral_id,
                                          struct cfn_hal_clock_s   *clock,
                                          void                     *dependency,
                                          const cfn_hal_gpio_api_t *api,
                                          const cfn_hal_gpio_phy_t *phy,
                                          cfn_hal_gpio_callback_t   callback,
                                          void                     *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_HAL_PERIPHERAL_TYPE_GPIO, peripheral_id, clock, dependency, api, phy, NULL, callback, user_arg);
}

/**
 * @brief Validates the GPIO pin configuration.
 * @param driver Pointer to the GPIO driver instance.
 * @param pin_cfg Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_pin_config_validate(const cfn_hal_gpio_t            *driver,
                                                                     const cfn_hal_gpio_pin_config_t *pin_cfg)
{
    if (driver == NULL || pin_cfg == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (pin_cfg->mode >= CFN_HAL_GPIO_CONFIG_MODE_MAX || pin_cfg->pull >= CFN_HAL_GPIO_CONFIG_PULL_MAX ||
        pin_cfg->speed >= CFN_HAL_GPIO_CONFIG_SPEED_MAX || pin_cfg->strength >= CFN_HAL_GPIO_CONFIG_STRENGTH_MAX ||
        pin_cfg->default_state >= CFN_HAL_GPIO_STATE_MAX)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, pin_cfg);
}

/**
 * @brief Initializes the GPIO driver.
 * @param driver Pointer to the GPIO driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_init(cfn_hal_gpio_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO);
}

/**
 * @brief Deinitializes the GPIO driver.
 * @param driver Pointer to the GPIO driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_deinit(cfn_hal_gpio_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO);
}

/**
 * @brief Registers a callback for GPIO events and errors.
 * @param driver Pointer to the GPIO driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_callback_register(cfn_hal_gpio_t               *driver,
                                                                   const cfn_hal_gpio_callback_t callback,
                                                                   void                         *user_arg)
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the GPIO power state.
 * @param driver Pointer to the GPIO driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_power_state_set(cfn_hal_gpio_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, state);
}

/**
 * @brief Enables one or more GPIO nominal events.
 * @param driver Pointer to the GPIO driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_event_enable(cfn_hal_gpio_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, event_mask);
}

/**
 * @brief Disables one or more GPIO nominal events.
 * @param driver Pointer to the GPIO driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_event_disable(cfn_hal_gpio_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, event_mask);
}

/**
 * @brief Retrieves the current GPIO nominal event status.
 * @param driver Pointer to the GPIO driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_event_get(cfn_hal_gpio_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, event_mask);
}

/**
 * @brief Enables one or more GPIO exception errors.
 * @param driver Pointer to the GPIO driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_error_enable(cfn_hal_gpio_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, error_mask);
}

/**
 * @brief Disables one or more GPIO exception errors.
 * @param driver Pointer to the GPIO driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_error_disable(cfn_hal_gpio_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, error_mask);
}

/**
 * @brief Retrieves the current GPIO exception error status.
 * @param driver Pointer to the GPIO driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_error_get(cfn_hal_gpio_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_GPIO, error_mask);
}

/* GPIO Specific Functions ------------------------------------------ */

/**
 * @brief Configures specific pins on the port using a payload structure.
 * @param port Pointer to the GPIO port driver instance.
 * @param pin_cfg Pointer to the configuration payload containing the pin_mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_pin_config(cfn_hal_gpio_t                  *port,
                                                            const cfn_hal_gpio_pin_config_t *pin_cfg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, pin_config, port, error, pin_cfg);
    return error;
}

/**
 * @brief Reads the logical state of a single GPIO pin.
 * @param port Pointer to the GPIO port driver instance.
 * @param pin A single pin identifier (MUST NOT be a bitmask of multiple pins).
 * @param state [out] Pointer to store the read state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_pin_read(cfn_hal_gpio_t       *port,
                                                          cfn_hal_gpio_pin_t    pin,
                                                          cfn_hal_gpio_state_t *state)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, pin_read, port, error, pin, state);
    return error;
}

/**
 * @brief Writes a logical state to a single GPIO pin.
 * @param port Pointer to the GPIO port driver instance.
 * @param pin A single pin identifier.
 * @param state The state value to write.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_pin_write(cfn_hal_gpio_t      *port,
                                                           cfn_hal_gpio_pin_t   pin,
                                                           cfn_hal_gpio_state_t state)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, pin_write, port, error, pin, state);
    return error;
}

/**
 * @brief Toggles the logical state of a single GPIO pin.
 * @param port Pointer to the GPIO port driver instance.
 * @param pin A single pin identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_pin_toggle(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, pin_toggle, port, error, pin);
    return error;
}

/**
 * @brief Reads the 32-bit raw value of the entire GPIO port.
 * @param port Pointer to the GPIO port driver instance.
 * @param port_value [out] Pointer to store the 32-bit port value.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_port_read(cfn_hal_gpio_t *port, uint32_t *port_value)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, port_read, port, error, port_value);
    return error;
}

/**
 * @brief Atomically writes to multiple pins on the port using a mask.
 * @param port Pointer to the GPIO port driver instance.
 * @param pin_mask A bitmask of pins to modify.
 * @param port_value The 32-bit value to apply to the masked pins.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_port_write(cfn_hal_gpio_t *port,
                                                            uint32_t        pin_mask,
                                                            uint32_t        port_value)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_GPIO, port_write, port, error, pin_mask, port_value);
    return error;
}

/**
 * @brief Handle a hardware interrupt for the GPIO port.
 * @param port Pointer to the GPIO port driver instance.
 */
CFN_HAL_INLINE void cfn_hal_gpio_handle_interrupt(cfn_hal_gpio_t *port)
{
    if (port && port->api && port->api->extender && port->api->extender->handle_interrupt)
    {
        port->api->extender->handle_interrupt(port);
    }
}

/**
 * @brief Get the ID of the GPIO expander.
 * @param port Pointer to the GPIO port driver instance.
 * @param id_out Pointer to store the ID.
 * @return CFN_HAL_ERROR_OK on success, CFN_HAL_ERROR_NOT_SUPPORTED if not an expander.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_gpio_get_id(cfn_hal_gpio_t *port, uint32_t *id_out)
{
    if (!port || !port->api || !id_out)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    if (port->api->extender && port->api->extender->get_id)
    {
        return port->api->extender->get_id(port, id_out);
    }
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t           *driver,
                                            const cfn_hal_gpio_phy_t *phy,
                                            struct cfn_hal_clock_s   *clock,
                                            void                     *dependency,
                                            cfn_hal_gpio_callback_t   callback,
                                            void                     *user_arg);
cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_GPIO_H
