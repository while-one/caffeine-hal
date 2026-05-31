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
 * @file cfn_hal_sdio.h
 * @brief SDIO / SDMMC HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_SDIO_H
#define CAFFEINE_HAL_HAL_SDIO_H

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
 * @brief SDIO nominal event flags.
 */
typedef enum
{
    CFN_HAL_SDIO_EVENT_NONE        = 0,
    CFN_HAL_SDIO_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Block transmission finished */
    CFN_HAL_SDIO_EVENT_RX_READY    = CFN_HAL_BIT(1), /*!< Block reception finished */
    CFN_HAL_SDIO_EVENT_CMD_SENT    = CFN_HAL_BIT(2), /*!< Command sent and response received */
} cfn_hal_sdio_event_t;

/**
 * @brief SDIO exception error flags.
 */
typedef enum
{
    CFN_HAL_SDIO_ERROR_NONE    = 0,
    CFN_HAL_SDIO_ERROR_CRC     = CFN_HAL_BIT(0), /*!< CRC mismatch in command or data */
    CFN_HAL_SDIO_ERROR_TIMEOUT = CFN_HAL_BIT(1), /*!< Command or data timeout */
    CFN_HAL_SDIO_ERROR_GENERAL = CFN_HAL_BIT(2), /*!< General hardware error */
} cfn_hal_sdio_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief SDIO command configuration.
 */
typedef struct
{
    uint32_t arg;           /*!< Command argument */
    uint32_t cmd_index;     /*!< Command identifier (CMD0-CMD63) */
    uint32_t response_type; /*!< Expected response length/format */
    uint32_t wait_type;     /*!< Interrupt or polling wait strategy */
} cfn_hal_sdio_cmd_t;

/**
 * @brief SDIO card information.
 */
typedef struct
{
    uint32_t card_type;     /*!< SDSC, SDHC, SDXC, etc. */
    uint32_t card_version;  /*!< Hardware revision */
    uint32_t speed_class;   /*!< Speed class */
    uint32_t rel_card_addr; /*!< Relative Card Address (RCA) */
    uint32_t block_count;   /*!< Total capacity in blocks */
    uint32_t block_size;    /*!< Block size in bytes (usually 512) */
    uint32_t log_block_count;
    uint32_t log_block_size;
} cfn_hal_sdio_card_info_t;

/**
 * @brief SDIO configuration structure.
 */
typedef struct
{
    uint32_t bus_wide;   /*!< 1-bit, 4-bit, or 8-bit bus mode */
    uint32_t clock_freq; /*!< SDIO bus clock frequency */
    void    *custom;     /*!< Vendor-specific custom configuration */
} cfn_hal_sdio_config_t;

/**
 * @brief SDIO hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    cfn_hal_gpio_pin_handle_t *ck;       /*!< Clock mapping */
    cfn_hal_gpio_pin_handle_t *cmd;      /*!< Command mapping */
    cfn_hal_gpio_pin_handle_t *d0;       /*!< Data 0 mapping */
    cfn_hal_gpio_pin_handle_t *d1;       /*!< Data 1 mapping */
    cfn_hal_gpio_pin_handle_t *d2;       /*!< Data 2 mapping */
    cfn_hal_gpio_pin_handle_t *d3;       /*!< Data 3 mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_sdio_phy_t;

typedef struct cfn_hal_sdio_s     cfn_hal_sdio_t;
typedef struct cfn_hal_sdio_api_s cfn_hal_sdio_api_t;

/**
 * @brief SDIO callback signature.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param p_user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_sdio_callback_t)(cfn_hal_sdio_t *p_driver,
                                        uint32_t        event_mask,
                                        uint32_t        error_mask,
                                        void           *p_user_arg);

/**
 * @brief SDIO Virtual Method Table (VMT).
 */
struct cfn_hal_sdio_api_s
{
    cfn_hal_api_base_t base;

    /* SDIO Specific Extensions */
    cfn_hal_error_code_t (*send_command)(cfn_hal_sdio_t           *p_driver,
                                         const cfn_hal_sdio_cmd_t *p_cmd,
                                         uint32_t                 *p_response);
    cfn_hal_error_code_t (*read_blocks)(
        cfn_hal_sdio_t *p_driver, uint8_t *p_buffer, uint32_t block_addr, uint32_t nbr_of_blocks, uint32_t timeout);
    cfn_hal_error_code_t (*write_blocks)(
        cfn_hal_sdio_t *p_driver, const uint8_t *p_data, uint32_t block_addr, uint32_t nbr_of_blocks, uint32_t timeout);
    cfn_hal_error_code_t (*get_card_info)(cfn_hal_sdio_t *p_driver, cfn_hal_sdio_card_info_t *p_info);
    cfn_hal_error_code_t (*wait_card_ready)(cfn_hal_sdio_t *p_driver, uint32_t timeout);
};
CFN_HAL_VMT_CHECK(struct cfn_hal_sdio_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    sdio, cfn_hal_sdio_config_t, cfn_hal_sdio_api_t, cfn_hal_sdio_phy_t, cfn_hal_sdio_callback_t);

/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void
cfn_hal_sdio_populate (cfn_hal_sdio_t              *p_driver,
                       uint32_t                     peripheral_id,
                       struct cfn_hal_clock_s      *p_clock,
                       void                        *p_dependency,
                       const cfn_hal_sdio_api_t    *p_api,
                       const cfn_hal_sdio_phy_t    *p_phy,
                       const cfn_hal_sdio_config_t *p_config,
                       cfn_hal_sdio_callback_t      p_callback,
                       void                        *p_user_arg)
{
    CFN_HAL_POPULATE_DRIVER(p_driver,
                            CFN_HAL_PERIPHERAL_TYPE_SDIO,
                            peripheral_id,
                            p_clock,
                            p_dependency,
                            p_api,
                            p_phy,
                            p_config,
                            p_callback,
                            p_user_arg);
}

/**
 * @brief Validates the SDIO configuration.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_config_validate (const cfn_hal_sdio_t *p_driver, const cfn_hal_sdio_config_t *p_config)
{
    if (p_driver == NULL || p_config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, p_config);
}

/**
 * @brief Initializes the SDIO driver.
 * @param p_driver Pointer to the SDIO driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_init (cfn_hal_sdio_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    p_driver->base.vmt         = (const struct cfn_hal_api_base_s *) p_driver->api;
    cfn_hal_error_code_t error = cfn_hal_sdio_config_validate(p_driver, p_driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO);
}

/**
 * @brief Deinitializes the SDIO driver.
 * @param p_driver Pointer to the SDIO driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_deinit (cfn_hal_sdio_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO);
}

/**
 * @brief Sets the SDIO configuration.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_config_set (cfn_hal_sdio_t *p_driver, const cfn_hal_sdio_config_t *p_config)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_sdio_config_validate(p_driver, p_config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        p_driver->config = p_config;
    }
    return cfn_hal_base_config_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, (const void *) p_config);
}

/**
 * @brief Gets the current SDIO configuration.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_config_get (cfn_hal_sdio_t *p_driver, cfn_hal_sdio_config_t *p_config)
{
    if (!p_driver || !p_config || !p_driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *p_config = *(p_driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for SDIO events and errors.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param callback The callback function to register.
 * @param p_user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_callback_register (cfn_hal_sdio_t *p_driver, const cfn_hal_sdio_callback_t callback, void *p_user_arg)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        p_driver->cb          = callback;
        p_driver->cb_user_arg = p_user_arg;
    }
    return cfn_hal_base_callback_register(
        &p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, (cfn_hal_callback_t) callback, p_user_arg);
}

/**
 * @brief Sets the SDIO power state.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_power_state_set (cfn_hal_sdio_t *p_driver, cfn_hal_power_state_t state)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, state);
}

/**
 * @brief Enables one or more SDIO nominal events.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_event_enable (cfn_hal_sdio_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, event_mask);
}

/**
 * @brief Disables one or more SDIO nominal events.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_event_disable (cfn_hal_sdio_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, event_mask);
}

/**
 * @brief Retrieves the current SDIO nominal event status.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_event_get (cfn_hal_sdio_t *p_driver, uint32_t *p_event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, p_event_mask);
}

/**
 * @brief Enables one or more SDIO exception errors.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_error_enable (cfn_hal_sdio_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, error_mask);
}

/**
 * @brief Disables one or more SDIO exception errors.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_error_disable (cfn_hal_sdio_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, error_mask);
}

/**
 * @brief Retrieves the current SDIO exception error status.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_error_get (cfn_hal_sdio_t *p_driver, uint32_t *p_error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_SDIO, p_error_mask);
}

/* SDIO Specific Functions ------------------------------------------ */

/**
 * @brief Sends an SD/MMC command and waits for a response.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_cmd Pointer to the command configuration structure.
 * @param p_response [out] Pointer to store the card response.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_send_command (cfn_hal_sdio_t *p_driver, const cfn_hal_sdio_cmd_t *p_cmd, uint32_t *p_response)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SDIO, send_command, p_driver, error, p_cmd, p_response);
    return error;
}

/**
 * @brief Reads data blocks from the SD card.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_buffer Pointer to the destination memory.
 * @param block_addr Address of the first block to read.
 * @param nbr_of_blocks Number of blocks to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_read_blocks (
    cfn_hal_sdio_t *p_driver, uint8_t *p_buffer, uint32_t block_addr, uint32_t nbr_of_blocks, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_SDIO, read_blocks, p_driver, error, p_buffer, block_addr, nbr_of_blocks, timeout);
    return error;
}

/**
 * @brief Writes data blocks to the SD card.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_data Pointer to the source data.
 * @param block_addr Address of the first block to write.
 * @param nbr_of_blocks Number of blocks to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_write_blocks (
    cfn_hal_sdio_t *p_driver, const uint8_t *p_data, uint32_t block_addr, uint32_t nbr_of_blocks, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_SDIO, write_blocks, p_driver, error, p_data, block_addr, nbr_of_blocks, timeout);
    return error;
}

/**
 * @brief Retrieves card-specific registers and capacity.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param p_info [out] Pointer to the card info structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_get_card_info (cfn_hal_sdio_t *p_driver, cfn_hal_sdio_card_info_t *p_info)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SDIO, get_card_info, p_driver, error, p_info);
    return error;
}

/**
 * @brief Waits until the SD card is no longer busy.
 * @param p_driver Pointer to the SDIO driver instance.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_sdio_wait_card_ready (cfn_hal_sdio_t *p_driver, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SDIO, wait_card_ready, p_driver, error, timeout);
    return error;
}
cfn_hal_error_code_t cfn_hal_sdio_construct (cfn_hal_sdio_t              *p_driver,
                                             const cfn_hal_sdio_config_t *p_config,
                                             const cfn_hal_sdio_phy_t    *p_phy,
                                             struct cfn_hal_clock_s      *p_clock,
                                             void                        *p_dependency,
                                             cfn_hal_sdio_callback_t      p_callback,
                                             void                        *p_user_arg);
cfn_hal_error_code_t cfn_hal_sdio_destruct (cfn_hal_sdio_t *p_driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_SDIO_H
