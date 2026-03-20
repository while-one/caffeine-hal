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
 * @file cfn_hal_i2c.h
 * @brief Inter-Integrated Circuit (I2C) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_I2C_H
#define CAFFEINE_HAL_HAL_I2C_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_types.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief I2C nominal event flags.
 */
typedef enum
{
    CFN_HAL_I2C_EVENT_NONE        = 0,
    CFN_HAL_I2C_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Transmission complete */
    CFN_HAL_I2C_EVENT_RX_READY    = CFN_HAL_BIT(1), /*!< Data received and ready */
    CFN_HAL_I2C_EVENT_START_SENT  = CFN_HAL_BIT(2), /*!< Start condition generated */
    CFN_HAL_I2C_EVENT_ADDR_SENT   = CFN_HAL_BIT(3), /*!< Address phase complete */
    CFN_HAL_I2C_EVENT_HEADER_SENT = CFN_HAL_BIT(4), /*!< 10-bit header sent */
    CFN_HAL_I2C_EVENT_DATA_SENT   = CFN_HAL_BIT(5), /*!< Single byte transfer complete */
    CFN_HAL_I2C_EVENT_STOP_DET    = CFN_HAL_BIT(6), /*!< Stop condition detected */
} cfn_hal_i2c_event_t;

/**
 * @brief I2C exception error flags.
 */
typedef enum
{
    CFN_HAL_I2C_ERROR_NONE        = 0,
    CFN_HAL_I2C_ERROR_ARBITRATION = CFN_HAL_BIT(0), /*!< Arbitration lost */
    CFN_HAL_I2C_ERROR_ACK         = CFN_HAL_BIT(1), /*!< Acknowledge failure */
    CFN_HAL_I2C_ERROR_OVERRUN     = CFN_HAL_BIT(2), /*!< Data overrun/underrun */
    CFN_HAL_I2C_ERROR_PEC         = CFN_HAL_BIT(3), /*!< Packet Error Checking mismatch */
    CFN_HAL_I2C_ERROR_TIMEOUT     = CFN_HAL_BIT(4), /*!< Bus timeout */
    CFN_HAL_I2C_ERROR_SMB         = CFN_HAL_BIT(5), /*!< SMBus alert/error */
    CFN_HAL_I2C_ERROR_GENERAL     = CFN_HAL_BIT(6), /*!< General hardware error */
} cfn_hal_i2c_error_t;

/**
 * @brief I2C bus speeds.
 */
typedef enum
{
    CFN_HAL_I2C_CONFIG_SPEED_100KHZ,  /*!< Standard Mode */
    CFN_HAL_I2C_CONFIG_SPEED_400KHZ,  /*!< Fast Mode */
    CFN_HAL_I2C_CONFIG_SPEED_1000KHZ, /*!< Fast Mode Plus */
    CFN_HAL_I2C_CONFIG_SPEED_3330KHZ, /*!< High Speed (3.33 MHz) */
    CFN_HAL_I2C_CONFIG_SPEED_3400KHZ, /*!< High Speed (3.40 MHz) */
    CFN_HAL_I2C_CONFIG_SPEED_5000KHZ, /*!< Ultra Fast Mode */
    CFN_HAL_I2C_CONFIG_SPEED_CUSTOM,  /*!< Vendor-specific speed */

    CFN_HAL_I2C_CONFIG_SPEED_MAX
} cfn_hal_i2c_config_speed_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief I2C hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    cfn_hal_gpio_pin_handle_t *sda;      /*!< Serial Data pin mapping */
    cfn_hal_gpio_pin_handle_t *scl;      /*!< Serial Clock pin mapping */
    cfn_hal_gpio_pin_handle_t *irq;      /*!< Optional external IRQ pin */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_i2c_phy_t;

/**
 * @brief I2C configuration structure.
 */
typedef struct cfn_hal_i2c_config_s
{
    cfn_hal_i2c_config_speed_t speed;  /*!< I2C bus speed */
    void                      *custom; /*!< Vendor-specific custom configuration */
} cfn_hal_i2c_config_t;

/**
 * @brief Standard I2C transaction structure.
 */
typedef struct
{
    uint16_t       slave_address;   /*!< 7-bit or 10-bit target address */
    const uint8_t *tx_payload;      /*!< Buffer for outgoing data */
    size_t         nbr_of_tx_bytes; /*!< Bytes to transmit */
    uint8_t       *rx_payload;      /*!< Buffer for incoming data */
    size_t         nbr_of_rx_bytes; /*!< Bytes to receive */
} cfn_hal_i2c_transaction_t;

/**
 * @brief I2C memory access transaction structure.
 */
typedef struct
{
    uint16_t dev_addr;      /*!< Target device address */
    uint16_t mem_addr;      /*!< Internal memory address */
    uint8_t  mem_addr_size; /*!< Size of memory address in bytes (1 or 2) */
    uint8_t *data;          /*!< Data buffer for read or write */
    size_t   size;          /*!< Bytes to transfer */
} cfn_hal_i2c_mem_transaction_t;

typedef struct cfn_hal_i2c_s     cfn_hal_i2c_t;
typedef struct cfn_hal_i2c_api_s cfn_hal_i2c_api_t;

/**
 * @brief I2C callback signature.
 * @param driver Pointer to the I2C driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param payload Pointer to the transaction buffer (if applicable).
 * @param nbr_of_bytes Number of bytes transferred.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_i2c_callback_t)(cfn_hal_i2c_t *driver,
                                       uint32_t       event_mask,
                                       uint32_t       error_mask,
                                       uint8_t       *payload,
                                       size_t         nbr_of_bytes,
                                       void          *user_arg);

/**
 * @brief I2C Virtual Method Table (VMT).
 */
struct cfn_hal_i2c_api_s
{
    cfn_hal_api_base_t base;

    /* I2C Specific Extensions */
    cfn_hal_error_code_t (*xfr_irq)(cfn_hal_i2c_t *driver, const cfn_hal_i2c_transaction_t *xfr);
    cfn_hal_error_code_t (*xfr_irq_abort)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*xfr_polling)(cfn_hal_i2c_t *driver, const cfn_hal_i2c_transaction_t *xfr, uint32_t timeout);
    cfn_hal_error_code_t (*mem_read)(cfn_hal_i2c_t                       *driver,
                                     const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                                     uint32_t                             timeout);
    cfn_hal_error_code_t (*mem_write)(cfn_hal_i2c_t                       *driver,
                                      const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                                      uint32_t                             timeout);
    cfn_hal_error_code_t (*xfr_dma)(cfn_hal_i2c_t *driver, const cfn_hal_i2c_transaction_t *xfr);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_i2c_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(i2c, cfn_hal_i2c_config_t, cfn_hal_i2c_api_t, cfn_hal_i2c_phy_t, cfn_hal_i2c_callback_t);

/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void cfn_hal_i2c_populate(cfn_hal_i2c_t *driver,
                                          struct cfn_hal_clock_s *clock,
                                          const cfn_hal_i2c_api_t *api,
                                          const cfn_hal_i2c_phy_t *phy,
                                          const cfn_hal_i2c_config_t *config,
                                          cfn_hal_i2c_callback_t callback,
                                          void *user_arg) {
  if (!driver) return;
  cfn_hal_base_populate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C,
                        &api->base, clock);
  driver->api = api;
  driver->phy = phy;
  driver->config = config;
  driver->cb = callback;
  driver->cb_user_arg = user_arg;
}


/**
 * @brief Validates the I2C configuration.
 * @param driver Pointer to the I2C driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_config_validate(const cfn_hal_i2c_t        *driver,
                                                                const cfn_hal_i2c_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (config->speed >= CFN_HAL_I2C_CONFIG_SPEED_MAX)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, config);
}

/**
 * @brief Initializes the I2C driver.
 * @param driver Pointer to the I2C driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_init(cfn_hal_i2c_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt           = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_hal_i2c_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C);
}

/**
 * @brief Deinitializes the I2C driver.
 * @param driver Pointer to the I2C driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_deinit(cfn_hal_i2c_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C);
}

/**
 * @brief Sets the I2C configuration.
 * @param driver Pointer to the I2C driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_config_set(cfn_hal_i2c_t *driver, const cfn_hal_i2c_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_i2c_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, (const void *) config);
}

/**
 * @brief Gets the current I2C configuration.
 * @param driver Pointer to the I2C driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_config_get(cfn_hal_i2c_t *driver, cfn_hal_i2c_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for I2C events and errors.
 * @param driver Pointer to the I2C driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_callback_register(cfn_hal_i2c_t               *driver,
                                                                  const cfn_hal_i2c_callback_t callback,
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the I2C power state.
 * @param driver Pointer to the I2C driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_power_state_set(cfn_hal_i2c_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, state);
}

/**
 * @brief Enables one or more I2C nominal events.
 * @param driver Pointer to the I2C driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_event_enable(cfn_hal_i2c_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, event_mask);
}

/**
 * @brief Disables one or more I2C nominal events.
 * @param driver Pointer to the I2C driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_event_disable(cfn_hal_i2c_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, event_mask);
}

/**
 * @brief Retrieves the current I2C nominal event status.
 * @param driver Pointer to the I2C driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_event_get(cfn_hal_i2c_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, event_mask);
}

/**
 * @brief Enables one or more I2C exception errors.
 * @param driver Pointer to the I2C driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_error_enable(cfn_hal_i2c_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, error_mask);
}

/**
 * @brief Disables one or more I2C exception errors.
 * @param driver Pointer to the I2C driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_error_disable(cfn_hal_i2c_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, error_mask);
}

/**
 * @brief Retrieves the current I2C exception error status.
 * @param driver Pointer to the I2C driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_error_get(cfn_hal_i2c_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2C, error_mask);
}

/* I2C Specific Functions ------------------------------------------- */

/**
 * @brief Starts an I2C transaction using interrupts (non-blocking).
 * @param driver Pointer to the I2C driver instance.
 * @param xfr Pointer to the transaction structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_xfr_irq(cfn_hal_i2c_t *driver, const cfn_hal_i2c_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, xfr_irq, driver, error, xfr);
    return error;
}

/**
 * @brief Aborts an ongoing non-blocking I2C transaction.
 * @param driver Pointer to the I2C driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_xfr_irq_abort(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, xfr_irq_abort, driver, error);
    return error;
}

/**
 * @brief Executes an I2C transaction using polling (blocking).
 * @param driver Pointer to the I2C driver instance.
 * @param xfr Pointer to the transaction structure.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_xfr_polling(cfn_hal_i2c_t                   *driver,
                                                            const cfn_hal_i2c_transaction_t *xfr,
                                                            uint32_t                         timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, xfr_polling, driver, error, xfr, timeout);
    return error;
}

/**
 * @brief Reads data from a specific memory address on an I2C device (blocking).
 * @param driver Pointer to the I2C driver instance.
 * @param mem_xfr Pointer to the memory transaction structure.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_mem_read(cfn_hal_i2c_t                       *driver,
                                                         const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                                                         uint32_t                             timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, mem_read, driver, error, mem_xfr, timeout);
    return error;
}

/**
 * @brief Writes data to a specific memory address on an I2C device (blocking).
 * @param driver Pointer to the I2C driver instance.
 * @param mem_xfr Pointer to the memory transaction structure.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_mem_write(cfn_hal_i2c_t                       *driver,
                                                          const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                                                          uint32_t                             timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, mem_write, driver, error, mem_xfr, timeout);
    return error;
}

/**
 * @brief Starts an I2C transaction using DMA (non-blocking).
 * @param driver Pointer to the I2C driver instance.
 * @param xfr Pointer to the transaction structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2c_xfr_dma(cfn_hal_i2c_t *driver, const cfn_hal_i2c_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, xfr_dma, driver, error, xfr);
    return error;
}
cfn_hal_error_code_t cfn_hal_i2c_construct(cfn_hal_i2c_t *driver, const cfn_hal_i2c_config_t *config, const cfn_hal_i2c_phy_t *phy, struct cfn_hal_clock_s *clock, cfn_hal_i2c_callback_t callback, void *user_arg);
cfn_hal_error_code_t cfn_hal_i2c_destruct(cfn_hal_i2c_t *driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_I2C_H
