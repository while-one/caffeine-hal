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
 * @file cfn_hal_qspi.h
 * @brief Quad-SPI (QSPI / OSPI) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_QSPI_H
#define CAFFEINE_HAL_HAL_QSPI_H

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
 * @brief QSPI nominal event flags.
 */
typedef enum
{
    CFN_HAL_QSPI_EVENT_NONE = 0,
    CFN_HAL_QSPI_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Data transmission finished */
    CFN_HAL_QSPI_EVENT_RX_READY = CFN_HAL_BIT(1),    /*!< Data reception finished */
    CFN_HAL_QSPI_EVENT_CMD_DONE = CFN_HAL_BIT(2),    /*!< Command phase complete */
} cfn_hal_qspi_event_t;

/**
 * @brief QSPI exception error flags.
 */
typedef enum
{
    CFN_HAL_QSPI_ERROR_NONE = 0,
    CFN_HAL_QSPI_ERROR_TIMEOUT = CFN_HAL_BIT(0), /*!< Peripheral or memory timeout */
    CFN_HAL_QSPI_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_qspi_error_t;

/**
 * @brief QSPI bus width modes.
 */
typedef enum
{
    CFN_HAL_QSPI_BUS_WIDTH_NONE = 0,
    CFN_HAL_QSPI_BUS_WIDTH_SINGLE = 1,
    CFN_HAL_QSPI_BUS_WIDTH_DUAL = 2,
    CFN_HAL_QSPI_BUS_WIDTH_QUAD = 3,
} cfn_hal_qspi_bus_width_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief QSPI command configuration.
 */
typedef struct
{
    uint32_t                 instruction;      /*!< Instruction to be sent */
    uint32_t                 address;          /*!< Memory address */
    uint32_t                 dummy_cycles;     /*!< Number of dummy cycles */
    cfn_hal_qspi_bus_width_t instruction_mode; /*!< Instruction bus width */
    cfn_hal_qspi_bus_width_t address_mode;     /*!< Address bus width */
    cfn_hal_qspi_bus_width_t data_mode;        /*!< Data bus width */
    size_t                   nbr_of_data;      /*!< Number of bytes expected in data phase */
} cfn_hal_qspi_cmd_t;

/**
 * @brief QSPI configuration structure.
 */
typedef struct
{
    uint32_t clock_prescaler; /*!< SPI clock division factor */
    uint32_t flash_size;      /*!< Memory size in bytes */
    void    *custom;          /*!< Vendor-specific custom configuration */
} cfn_hal_qspi_config_t;

/**
 * @brief QSPI hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_qspi_phy_t;

typedef struct cfn_hal_qspi_s     cfn_hal_qspi_t;
typedef struct cfn_hal_qspi_api_s cfn_hal_qspi_api_t;

/**
 * @brief QSPI callback signature.
 * @param driver Pointer to the QSPI driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_qspi_callback_t)(cfn_hal_qspi_t *driver,
                                        uint32_t        event_mask,
                                        uint32_t        error_mask,
                                        void           *user_arg);

/**
 * @brief QSPI Virtual Method Table (VMT).
 */
struct cfn_hal_qspi_api_s
{
    cfn_hal_api_base_t base;

    /* QSPI Specific Extensions */
    cfn_hal_error_code_t (*command)(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd, uint32_t timeout);
    cfn_hal_error_code_t (*transmit)(cfn_hal_qspi_t *driver, const uint8_t *data, uint32_t timeout);
    cfn_hal_error_code_t (*receive)(cfn_hal_qspi_t *driver, uint8_t *data, uint32_t timeout);
    cfn_hal_error_code_t (*memory_mapped_enable)(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd);
    cfn_hal_error_code_t (*autopolling_enable)(
        cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd, uint32_t match, uint32_t mask, uint32_t timeout);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_qspi_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    qspi, cfn_hal_qspi_config_t, cfn_hal_qspi_api_t, cfn_hal_qspi_phy_t, cfn_hal_qspi_callback_t);

#define CFN_HAL_QSPI_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                         \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_QSPI, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the QSPI driver.
 * @param driver Pointer to the QSPI driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_init(cfn_hal_qspi_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI);
}

/**
 * @brief Deinitializes the QSPI driver.
 * @param driver Pointer to the QSPI driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_deinit(cfn_hal_qspi_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI);
}

/**
 * @brief Sets the QSPI configuration.
 * @param driver Pointer to the QSPI driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_config_set(cfn_hal_qspi_t *driver, const cfn_hal_qspi_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, (const void *) config);
}

/**
 * @brief Gets the current QSPI configuration.
 * @param driver Pointer to the QSPI driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_config_get(cfn_hal_qspi_t *driver, cfn_hal_qspi_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for QSPI events and errors.
 * @param driver Pointer to the QSPI driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_callback_register(cfn_hal_qspi_t               *driver,
                                                                   const cfn_hal_qspi_callback_t callback,
                                                                   void                         *user_arg)
{
    if (driver)
    {
        driver->cb = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the QSPI power state.
 * @param driver Pointer to the QSPI driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_power_state_set(cfn_hal_qspi_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, state);
}

/**
 * @brief Enables one or more QSPI nominal events.
 * @param driver Pointer to the QSPI driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_event_enable(cfn_hal_qspi_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, event_mask);
}

/**
 * @brief Disables one or more QSPI nominal events.
 * @param driver Pointer to the QSPI driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_event_disable(cfn_hal_qspi_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, event_mask);
}

/**
 * @brief Retrieves the current QSPI nominal event status.
 * @param driver Pointer to the QSPI driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_event_get(cfn_hal_qspi_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, event_mask);
}

/**
 * @brief Enables one or more QSPI exception errors.
 * @param driver Pointer to the QSPI driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_error_enable(cfn_hal_qspi_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, error_mask);
}

/**
 * @brief Disables one or more QSPI exception errors.
 * @param driver Pointer to the QSPI driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_error_disable(cfn_hal_qspi_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, error_mask);
}

/**
 * @brief Retrieves the current QSPI exception error status.
 * @param driver Pointer to the QSPI driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_error_get(cfn_hal_qspi_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_QSPI, error_mask);
}

/* QSPI Specific Functions ------------------------------------------ */

/**
 * @brief Sends a command sequence to the memory device.
 * @param driver Pointer to the QSPI driver instance.
 * @param cmd Pointer to the command configuration structure.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_command(cfn_hal_qspi_t           *driver,
                                                         const cfn_hal_qspi_cmd_t *cmd,
                                                         uint32_t                  timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_QSPI, command, driver, error, cmd, timeout);
    return error;
}

/**
 * @brief Transmits data following a command phase.
 * @param driver Pointer to the QSPI driver instance.
 * @param data Pointer to the data to be sent.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_transmit(cfn_hal_qspi_t *driver, const uint8_t *data, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_QSPI, transmit, driver, error, data, timeout);
    return error;
}

/**
 * @brief Receives data following a command phase.
 * @param driver Pointer to the QSPI driver instance.
 * @param data [out] Pointer to the buffer where received data will be stored.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_receive(cfn_hal_qspi_t *driver, uint8_t *data, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_QSPI, receive, driver, error, data, timeout);
    return error;
}

/**
 * @brief Enables memory-mapped mode for direct CPU access.
 * @param driver Pointer to the QSPI driver instance.
 * @param cmd Pointer to the read command configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_memory_mapped_enable(cfn_hal_qspi_t           *driver,
                                                                      const cfn_hal_qspi_cmd_t *cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_QSPI, memory_mapped_enable, driver, error, cmd);
    return error;
}

/**
 * @brief Configures autonomous polling of a memory status register.
 * @param driver Pointer to the QSPI driver instance.
 * @param cmd Pointer to the status register read command.
 * @param match Value to wait for in the status register.
 * @param mask Mask to apply to the read value before comparison.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_qspi_autopolling_enable(
    cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd, uint32_t match, uint32_t mask, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_QSPI, autopolling_enable, driver, error, cmd, match, mask, timeout);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_QSPI_H
