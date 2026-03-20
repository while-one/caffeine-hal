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
 * @file cfn_hal_nvm.h
 * @brief NVM (Non-Volatile Memory) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_NVM_H
#define CAFFEINE_HAL_HAL_NVM_H

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
 * @brief NVM nominal event flags.
 */
typedef enum
{
    CFN_HAL_NVM_EVENT_NONE           = 0,
    CFN_HAL_NVM_EVENT_WRITE_COMPLETE = CFN_HAL_BIT(0), /*!< Page or byte write finished */
    CFN_HAL_NVM_EVENT_ERASE_COMPLETE = CFN_HAL_BIT(1), /*!< Sector or chip erase finished */
} cfn_hal_nvm_event_t;

/**
 * @brief NVM exception error flags.
 */
typedef enum
{
    CFN_HAL_NVM_ERROR_NONE      = 0,
    CFN_HAL_NVM_ERROR_PROTECTED = CFN_HAL_BIT(0), /*!< Access to protected area denied */
    CFN_HAL_NVM_ERROR_ALIGNMENT = CFN_HAL_BIT(1), /*!< Invalid address or size alignment */
    CFN_HAL_NVM_ERROR_GENERAL   = CFN_HAL_BIT(2), /*!< General hardware error */
} cfn_hal_nvm_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief NVM memory organization information.
 */
typedef struct
{
    size_t   total_size;   /*!< Total capacity in bytes */
    size_t   sector_size;  /*!< Smallest erasable unit in bytes */
    size_t   page_size;    /*!< Smallest programmable unit in bytes */
    uint32_t write_cycles; /*!< Endurance (rated cycles) */
} cfn_hal_nvm_info_t;

/**
 * @brief NVM configuration structure.
 */
typedef struct
{
    void *user_config; /*!< Vendor-specific controller configuration */
} cfn_hal_nvm_config_t;

/**
 * @brief NVM hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_nvm_phy_t;

typedef struct cfn_hal_nvm_s     cfn_hal_nvm_t;
typedef struct cfn_hal_nvm_api_s cfn_hal_nvm_api_t;

/**
 * @brief NVM callback signature.
 * @param driver Pointer to the NVM driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_nvm_callback_t)(cfn_hal_nvm_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief NVM Virtual Method Table (VMT).
 */
struct cfn_hal_nvm_api_s
{
    cfn_hal_api_base_t base;

    /* NVM Specific Extensions */
    cfn_hal_error_code_t (*read)(cfn_hal_nvm_t *driver, uint32_t addr, uint8_t *buffer, size_t size);
    cfn_hal_error_code_t (*write)(cfn_hal_nvm_t *driver, uint32_t addr, const uint8_t *data, size_t size);
    cfn_hal_error_code_t (*erase_sector)(cfn_hal_nvm_t *driver, uint32_t sector_addr);
    cfn_hal_error_code_t (*erase_chip)(cfn_hal_nvm_t *driver);
    cfn_hal_error_code_t (*get_info)(cfn_hal_nvm_t *driver, cfn_hal_nvm_info_t *info);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_nvm_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(nvm, cfn_hal_nvm_config_t, cfn_hal_nvm_api_t, cfn_hal_nvm_phy_t, cfn_hal_nvm_callback_t);
/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the NVM configuration.
 * @param driver Pointer to the NVM driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_config_validate(const cfn_hal_nvm_t        *driver,
                                                                const cfn_hal_nvm_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, config);
}

/**
 * @brief Initializes the NVM controller driver.
 * @param driver Pointer to the NVM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_init(cfn_hal_nvm_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt           = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_hal_nvm_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM);
}

/**
 * @brief Deinitializes the NVM controller driver.
 * @param driver Pointer to the NVM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_deinit(cfn_hal_nvm_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM);
}

/**
 * @brief Sets the NVM configuration.
 * @param driver Pointer to the NVM driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_config_set(cfn_hal_nvm_t *driver, const cfn_hal_nvm_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_nvm_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, (const void *) config);
}

/**
 * @brief Gets the current NVM configuration.
 * @param driver Pointer to the NVM driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_config_get(cfn_hal_nvm_t *driver, cfn_hal_nvm_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for NVM events and errors.
 * @param driver Pointer to the NVM driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_callback_register(cfn_hal_nvm_t               *driver,
                                                                  const cfn_hal_nvm_callback_t callback,
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the NVM power state.
 * @param driver Pointer to the NVM driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_power_state_set(cfn_hal_nvm_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, state);
}

/**
 * @brief Enables one or more NVM nominal events.
 * @param driver Pointer to the NVM driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_event_enable(cfn_hal_nvm_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, event_mask);
}

/**
 * @brief Disables one or more NVM nominal events.
 * @param driver Pointer to the NVM driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_event_disable(cfn_hal_nvm_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, event_mask);
}

/**
 * @brief Retrieves the current NVM nominal event status.
 * @param driver Pointer to the NVM driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_event_get(cfn_hal_nvm_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, event_mask);
}

/**
 * @brief Enables one or more NVM exception errors.
 * @param driver Pointer to the NVM driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_error_enable(cfn_hal_nvm_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, error_mask);
}

/**
 * @brief Disables one or more NVM exception errors.
 * @param driver Pointer to the NVM driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_error_disable(cfn_hal_nvm_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, error_mask);
}

/**
 * @brief Retrieves the current NVM exception error status.
 * @param driver Pointer to the NVM driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_error_get(cfn_hal_nvm_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, error_mask);
}

/* NVM Specific Functions ------------------------------------------- */

/**
 * @brief Reads data from non-volatile memory.
 * @param driver Pointer to the NVM driver instance.
 * @param addr Source memory address.
 * @param buffer Pointer to the buffer where data will be stored.
 * @param size Number of bytes to read.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_read(cfn_hal_nvm_t *driver, uint32_t addr, uint8_t *buffer, size_t size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, read, driver, error, addr, buffer, size);
    return error;
}

/**
 * @brief Programs data into non-volatile memory.
 * @param driver Pointer to the NVM driver instance.
 * @param addr Destination memory address.
 * @param data Pointer to the data to be written.
 * @param size Number of bytes to write.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_write(cfn_hal_nvm_t *driver,
                                                      uint32_t       addr,
                                                      const uint8_t *data,
                                                      size_t         size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, write, driver, error, addr, data, size);
    return error;
}

/**
 * @brief Erases a specific sector or page of memory.
 * @param driver Pointer to the NVM driver instance.
 * @param sector_addr Address within the sector to be erased.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_erase_sector(cfn_hal_nvm_t *driver, uint32_t sector_addr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, erase_sector, driver, error, sector_addr);
    return error;
}

/**
 * @brief Erases the entire non-volatile memory (Bulk Erase).
 * @param driver Pointer to the NVM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_erase_chip(cfn_hal_nvm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_NVM, erase_chip, driver, error);
    return error;
}

/**
 * @brief Retrieves information about memory organization and endurance.
 * @param driver Pointer to the NVM driver instance.
 * @param info [out] Pointer to the information structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_nvm_get_info(cfn_hal_nvm_t *driver, cfn_hal_nvm_info_t *info)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, get_info, driver, error, info);
    return error;
}
cfn_hal_error_code_t cfn_hal_nvm_construct(cfn_hal_nvm_t              *driver,
                                           const cfn_hal_nvm_config_t *config,
                                           const cfn_hal_nvm_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           cfn_hal_nvm_callback_t      callback,
                                           void                       *user_arg);
cfn_hal_error_code_t cfn_hal_nvm_destruct(cfn_hal_nvm_t *driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_NVM_H
