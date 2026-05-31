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
 * @param p_driver Pointer to the NVM driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param p_user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_nvm_callback_t)(cfn_hal_nvm_t *p_driver,
                                       uint32_t       event_mask,
                                       uint32_t       error_mask,
                                       void          *p_user_arg);

/**
 * @brief NVM Virtual Method Table (VMT).
 */
struct cfn_hal_nvm_api_s
{
    cfn_hal_api_base_t base;

    /* NVM Specific Extensions */
    cfn_hal_error_code_t (*read)(
        cfn_hal_nvm_t *p_driver, uint32_t addr, uint8_t *p_buffer, size_t size, uint32_t timeout);
    cfn_hal_error_code_t (*write)(
        cfn_hal_nvm_t *p_driver, uint32_t addr, const uint8_t *p_data, size_t size, uint32_t timeout);
    cfn_hal_error_code_t (*erase_sector)(cfn_hal_nvm_t *p_driver, uint32_t sector_addr, uint32_t timeout);
    cfn_hal_error_code_t (*erase_chip)(cfn_hal_nvm_t *p_driver, uint32_t timeout);
    cfn_hal_error_code_t (*get_info)(cfn_hal_nvm_t *p_driver, uint32_t addr, cfn_hal_nvm_info_t *p_info);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_nvm_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(nvm, cfn_hal_nvm_config_t, cfn_hal_nvm_api_t, cfn_hal_nvm_phy_t, cfn_hal_nvm_callback_t);
/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void
cfn_hal_nvm_populate (cfn_hal_nvm_t              *p_driver,
                      uint32_t                    peripheral_id,
                      struct cfn_hal_clock_s     *p_clock,
                      void                       *p_dependency,
                      const cfn_hal_nvm_api_t    *p_api,
                      const cfn_hal_nvm_phy_t    *p_phy,
                      const cfn_hal_nvm_config_t *p_config,
                      cfn_hal_nvm_callback_t      p_callback,
                      void                       *p_user_arg)
{
    CFN_HAL_POPULATE_DRIVER(p_driver,
                            CFN_HAL_PERIPHERAL_TYPE_NVM,
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
 * @brief Validates the NVM configuration.
 * @param p_driver Pointer to the NVM driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_config_validate (const cfn_hal_nvm_t *p_driver, const cfn_hal_nvm_config_t *p_config)
{
    if (p_driver == NULL || p_config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, p_config);
}

/**
 * @brief Initializes the NVM controller driver.
 * @param p_driver Pointer to the NVM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_init (cfn_hal_nvm_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    p_driver->base.vmt         = (const struct cfn_hal_api_base_s *) p_driver->api;
    cfn_hal_error_code_t error = cfn_hal_nvm_config_validate(p_driver, p_driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM);
}

/**
 * @brief Deinitializes the NVM controller driver.
 * @param p_driver Pointer to the NVM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_deinit (cfn_hal_nvm_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM);
}

/**
 * @brief Sets the NVM configuration.
 * @param p_driver Pointer to the NVM driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_config_set (cfn_hal_nvm_t *p_driver, const cfn_hal_nvm_config_t *p_config)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_nvm_config_validate(p_driver, p_config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        p_driver->config = p_config;
    }
    return cfn_hal_base_config_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, (const void *) p_config);
}

/**
 * @brief Gets the current NVM configuration.
 * @param p_driver Pointer to the NVM driver instance.
 * @param p_config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_config_get (cfn_hal_nvm_t *p_driver, cfn_hal_nvm_config_t *p_config)
{
    if (!p_driver || !p_config || !p_driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *p_config = *(p_driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for NVM events and errors.
 * @param p_driver Pointer to the NVM driver instance.
 * @param callback The callback function to register.
 * @param p_user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_callback_register (cfn_hal_nvm_t *p_driver, const cfn_hal_nvm_callback_t callback, void *p_user_arg)
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
        &p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, (cfn_hal_callback_t) callback, p_user_arg);
}

/**
 * @brief Sets the NVM power state.
 * @param p_driver Pointer to the NVM driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_power_state_set (cfn_hal_nvm_t *p_driver, cfn_hal_power_state_t state)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, state);
}

/**
 * @brief Enables one or more NVM nominal events.
 * @param p_driver Pointer to the NVM driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_event_enable (cfn_hal_nvm_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, event_mask);
}

/**
 * @brief Disables one or more NVM nominal events.
 * @param p_driver Pointer to the NVM driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_event_disable (cfn_hal_nvm_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, event_mask);
}

/**
 * @brief Retrieves the current NVM nominal event status.
 * @param p_driver Pointer to the NVM driver instance.
 * @param p_event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_event_get (cfn_hal_nvm_t *p_driver, uint32_t *p_event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, p_event_mask);
}

/**
 * @brief Enables one or more NVM exception errors.
 * @param p_driver Pointer to the NVM driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_error_enable (cfn_hal_nvm_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, error_mask);
}

/**
 * @brief Disables one or more NVM exception errors.
 * @param p_driver Pointer to the NVM driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_error_disable (cfn_hal_nvm_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, error_mask);
}

/**
 * @brief Retrieves the current NVM exception error status.
 * @param p_driver Pointer to the NVM driver instance.
 * @param p_error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_error_get (cfn_hal_nvm_t *p_driver, uint32_t *p_error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_NVM, p_error_mask);
}

/* NVM Specific Functions ------------------------------------------- */

/**
 * @brief Reads data from non-volatile memory.
 * @param p_driver Pointer to the NVM driver instance.
 * @param addr Source memory address.
 * @param p_buffer Pointer to the buffer where data will be stored.
 * @param size Number of bytes to read.
 * @param timeout Maximum time to wait for completion in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_read (cfn_hal_nvm_t *p_driver, uint32_t addr, uint8_t *p_buffer, size_t size, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, read, p_driver, error, addr, p_buffer, size, timeout);
    return error;
}

/**
 * @brief Programs data into non-volatile memory.
 * @param p_driver Pointer to the NVM driver instance.
 * @param addr Destination memory address.
 * @param p_data Pointer to the data to be written.
 * @param size Number of bytes to write.
 * @param timeout Maximum time to wait for completion in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_write (cfn_hal_nvm_t *p_driver, uint32_t addr, const uint8_t *p_data, size_t size, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, write, p_driver, error, addr, p_data, size, timeout);
    return error;
}

/**
 * @brief Erases a specific sector or page of memory.
 * @param p_driver Pointer to the NVM driver instance.
 * @param sector_addr Address within the sector to be erased.
 * @param timeout Maximum time to wait for completion in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_erase_sector (cfn_hal_nvm_t *p_driver, uint32_t sector_addr, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, erase_sector, p_driver, error, sector_addr, timeout);
    return error;
}

/**
 * @brief Erases the entire non-volatile memory (Bulk Erase).
 * @param p_driver Pointer to the NVM driver instance.
 * @param timeout Maximum time to wait for completion in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_erase_chip (cfn_hal_nvm_t *p_driver, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, erase_chip, p_driver, error, timeout);
    return error;
}

/**
 * @brief Retrieves information about memory organization and endurance for a specific address.
 * @param p_driver Pointer to the NVM driver instance.
 * @param addr Address within the target sector/region.
 * @param p_info [out] Pointer to the information structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_nvm_get_info (cfn_hal_nvm_t *p_driver, uint32_t addr, cfn_hal_nvm_info_t *p_info)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_NVM, get_info, p_driver, error, addr, p_info);
    return error;
}
cfn_hal_error_code_t cfn_hal_nvm_construct (cfn_hal_nvm_t              *p_driver,
                                            const cfn_hal_nvm_config_t *p_config,
                                            const cfn_hal_nvm_phy_t    *p_phy,
                                            struct cfn_hal_clock_s     *p_clock,
                                            void                       *p_dependency,
                                            cfn_hal_nvm_callback_t      p_callback,
                                            void                       *p_user_arg);
cfn_hal_error_code_t cfn_hal_nvm_destruct (cfn_hal_nvm_t *p_driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_NVM_H
