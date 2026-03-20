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
 * @file cfn_hal_dma.h
 * @brief DMA (Direct Memory Access) Hardware Abstraction Layer.
 */

#ifndef CAFFEINE_HAL_HAL_DMA_H
#define CAFFEINE_HAL_HAL_DMA_H

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
 * @brief DMA nominal event flags.
 */
typedef enum
{
    CFN_HAL_DMA_EVENT_NONE              = 0,
    CFN_HAL_DMA_EVENT_TRANSFER_HALF     = CFN_HAL_BIT(0), /*!< Half of data transferred */
    CFN_HAL_DMA_EVENT_TRANSFER_COMPLETE = CFN_HAL_BIT(1), /*!< All data transferred */
} cfn_hal_dma_event_t;

/**
 * @brief DMA exception error flags.
 */
typedef enum
{
    CFN_HAL_DMA_ERROR_NONE     = 0,
    CFN_HAL_DMA_ERROR_TRANSFER = CFN_HAL_BIT(0), /*!< Transfer or bus error */
    CFN_HAL_DMA_ERROR_GENERAL  = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_dma_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief DMA transfer configuration.
 */
typedef struct
{
    uint32_t src_addr; /*!< Source memory or peripheral address */
    uint32_t dst_addr; /*!< Destination memory or peripheral address */
    uint32_t length;   /*!< Number of data units to transfer */
} cfn_hal_dma_transfer_t;

/**
 * @brief DMA configuration structure.
 */
typedef struct
{
    uint32_t direction; /*!< Transfer direction (M2M, P2M, M2P) */
    uint32_t priority;  /*!< Channel priority level */
    void    *custom;    /*!< Vendor-specific custom configuration */
} cfn_hal_dma_config_t;

/**
 * @brief DMA hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Channel or stream specific user argument */
} cfn_hal_dma_phy_t;

typedef struct cfn_hal_dma_s     cfn_hal_dma_t;
typedef struct cfn_hal_dma_api_s cfn_hal_dma_api_t;

/**
 * @brief DMA callback signature.
 * @param driver Pointer to the DMA driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_dma_callback_t)(cfn_hal_dma_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief DMA Virtual Method Table (VMT).
 */
struct cfn_hal_dma_api_s
{
    cfn_hal_api_base_t base;

    /* DMA Specific Extensions */
    cfn_hal_error_code_t (*start)(cfn_hal_dma_t *driver, const cfn_hal_dma_transfer_t *transfer);
    cfn_hal_error_code_t (*stop)(cfn_hal_dma_t *driver);
};
CFN_HAL_VMT_CHECK(struct cfn_hal_dma_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(dma, cfn_hal_dma_config_t, cfn_hal_dma_api_t, cfn_hal_dma_phy_t, cfn_hal_dma_callback_t);

#define CFN_HAL_DMA_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_DMA, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the DMA configuration.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_config_validate(const cfn_hal_dma_t        *driver,
                                                                const cfn_hal_dma_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, config);
}

/**
 * @brief Initializes the DMA driver.
 * @param driver Pointer to the DMA driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_init(cfn_hal_dma_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA);
}

/**
 * @brief Deinitializes the DMA driver.
 * @param driver Pointer to the DMA driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_deinit(cfn_hal_dma_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA);
}

/**
 * @brief Sets the DMA configuration.
 * @param driver Pointer to the DMA driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_config_set(cfn_hal_dma_t *driver, const cfn_hal_dma_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, (const void *) config);
}

/**
 * @brief Gets the current DMA configuration.
 * @param driver Pointer to the DMA driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_config_get(cfn_hal_dma_t *driver, cfn_hal_dma_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for DMA events and errors.
 * @param driver Pointer to the DMA driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_callback_register(cfn_hal_dma_t               *driver,
                                                                  const cfn_hal_dma_callback_t callback,
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the DMA power state.
 * @param driver Pointer to the DMA driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_power_state_set(cfn_hal_dma_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, state);
}

/**
 * @brief Enables one or more DMA nominal events.
 * @param driver Pointer to the DMA driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_event_enable(cfn_hal_dma_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, event_mask);
}

/**
 * @brief Disables one or more DMA nominal events.
 * @param driver Pointer to the DMA driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_event_disable(cfn_hal_dma_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, event_mask);
}

/**
 * @brief Retrieves the current DMA nominal event status.
 * @param driver Pointer to the DMA driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_event_get(cfn_hal_dma_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, event_mask);
}

/**
 * @brief Enables one or more DMA exception errors.
 * @param driver Pointer to the DMA driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_error_enable(cfn_hal_dma_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, error_mask);
}

/**
 * @brief Disables one or more DMA exception errors.
 * @param driver Pointer to the DMA driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_error_disable(cfn_hal_dma_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, error_mask);
}

/**
 * @brief Retrieves the current DMA exception error status.
 * @param driver Pointer to the DMA driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_error_get(cfn_hal_dma_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DMA, error_mask);
}

/* DMA Specific Functions ------------------------------------------- */

/**
 * @brief Starts a DMA transfer.
 * @param driver Pointer to the DMA driver instance.
 * @param transfer Pointer to the transfer configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_start(cfn_hal_dma_t *driver, const cfn_hal_dma_transfer_t *transfer)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DMA, start, driver, error, transfer);
    return error;
}

/**
 * @brief Stops an ongoing DMA transfer.
 * @param driver Pointer to the DMA driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dma_stop(cfn_hal_dma_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_DMA, stop, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_DMA_H
