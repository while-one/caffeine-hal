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
 * @file cfn_hal_base.h
 * @brief Base driver Hardware Abstraction Layer declarations.
 */

#ifndef CAFFEINE_HAL_HAL_BASE_H
#define CAFFEINE_HAL_HAL_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_types.h"

/* Defines ----------------------------------------------------------*/

/**
 * @def CFN_HAL_BASE_API
 * @brief API visibility macro for the base driver.
 *
 * This macro controls whether the base functions are 'static inline' (default),
 * 'extern' (when linking against a pre-compiled library), or empty (when
 * compiling the library source).
 */
#if defined(CFN_HAL_COMPILE_BASE)
#define CFN_HAL_BASE_API
#elif defined(CFN_HAL_USE_LIB_BASE)
#define CFN_HAL_BASE_API extern
#else
#define CFN_HAL_BASE_API static inline
#endif

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Base API structure for all peripheral drivers.
 * Every peripheral-specific API struct MUST have this as its first member.
 */
typedef struct cfn_hal_api_base_s {
  cfn_hal_error_code_t (*init)(cfn_hal_driver_t *base);
  cfn_hal_error_code_t (*deinit)(cfn_hal_driver_t *base);

  cfn_hal_error_code_t (*power_state_set)(cfn_hal_driver_t *base,
                                          cfn_hal_power_state_t state);
  cfn_hal_error_code_t (*config_set)(cfn_hal_driver_t *base,
                                     const void *config);
  cfn_hal_error_code_t (*config_validate)(const cfn_hal_driver_t *base,
                                          const void *config);
  cfn_hal_error_code_t (*callback_register)(cfn_hal_driver_t *base,
                                            cfn_hal_callback_t callback,
                                            void *user_arg);

  cfn_hal_error_code_t (*event_enable)(cfn_hal_driver_t *base,
                                       uint32_t event_mask);
  cfn_hal_error_code_t (*event_disable)(cfn_hal_driver_t *base,
                                        uint32_t event_mask);
  cfn_hal_error_code_t (*event_get)(cfn_hal_driver_t *base,
                                    uint32_t *event_mask);

  cfn_hal_error_code_t (*error_enable)(cfn_hal_driver_t *base,
                                       uint32_t error_mask);
  cfn_hal_error_code_t (*error_disable)(cfn_hal_driver_t *base,
                                        uint32_t error_mask);
  cfn_hal_error_code_t (*error_get)(cfn_hal_driver_t *base,
                                    uint32_t *error_mask);

#if (CFN_HAL_USE_LOCK == 1)
  cfn_hal_error_code_t (*lock)(cfn_hal_driver_t *base, uint32_t timeout);
  cfn_hal_error_code_t (*unlock)(cfn_hal_driver_t *base);
#endif
} cfn_hal_api_base_t;

/**
 * @brief Compile-time check to ensure a peripheral API struct is compatible
 * with the base layer. All peripheral APIs must have 'cfn_hal_api_base_t base'
 * as their FIRST member.
 */
#define CFN_HAL_VMT_CHECK(api_struct_type)                                     \
  CFN_HAL_STATIC_ASSERT(                                                       \
      offsetof(api_struct_type, base) == 0,                                    \
      "cfn_hal_api_base_t must be the first member of the VMT struct")

/* Functions Prototypes ---------------------------------------------*/

/**
 * @brief Generic initialization for any driver.
 * Handles board-level hooks, type validation, and hardware-specific commitment.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_init(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type);

/**
 * @brief Generic deinitialization for any driver.
 * Returns the driver to the CONSTRUCTED state and releases board resources.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_deinit(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type);

/**
 * @brief Generic configuration setter for any driver.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param config Pointer to the peripheral-specific configuration structure.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_config_set(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    const void *config);

/**
 * @brief Generic validation for a configuration
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_config_validate(
    const cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    const void *config);
/**
 * @brief Generic callback registration for any driver.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param callback Generic callback function pointer.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_callback_register(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    cfn_hal_callback_t callback, void *user_arg);

/**
 * @brief Generic power state transition for any driver.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param state The target power state to transition to.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_power_state_set(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    cfn_hal_power_state_t state);

/**
 * @brief Generic power state getter.
 * Returns the current power state from the software shadow.
 *
 * @param base Pointer to the base driver structure.
 * @return Current cfn_hal_power_state_t.
 */
CFN_HAL_BASE_API cfn_hal_power_state_t
cfn_hal_power_state_get(const cfn_hal_driver_t *base);

/**
 * @brief Generic event enable for any driver.
 * Activates nominal hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param event_mask Pointer to a peripheral-specific event mask.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_event_enable(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    uint32_t event_mask);

/**
 * @brief Generic event disable for any driver.
 * Deactivates nominal hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param event_mask Pointer to a peripheral-specific event mask.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_event_disable(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    uint32_t event_mask);

/**
 * @brief Generic event status getter for any driver.
 * Retrieves the current nominal hardware triggers/flags.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param event_mask [out] Pointer to a buffer to receive the event status.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_event_get(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    uint32_t *event_mask);

/**
 * @brief Generic error enable for any driver.
 * Activates exception-flow hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param error_mask Pointer to a peripheral-specific error mask.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_error_enable(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    uint32_t error_mask);

/**
 * @brief Generic error disable for any driver.
 * Deactivates exception-flow hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param error_mask Pointer to a peripheral-specific error mask.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_error_disable(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    uint32_t error_mask);

/**
 * @brief Generic error status getter for any driver.
 * Retrieves current exception-flow hardware flags/errors.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param error_mask [out] Pointer to a buffer to receive the error status.
 * @return cfn_hal_error_code_t status code.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_error_get(
    cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type,
    uint32_t *error_mask);

#if (CFN_HAL_USE_LOCK == 1)
/**
 * @brief Concurrency lock for a driver instance.
 * @param base Pointer to the base driver structure.
 * @param timeout Lock acquisition timeout in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or an error code on failure.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_lock(cfn_hal_driver_t *base,
                                                        uint32_t timeout);

/**
 * @brief Concurrency unlock for a driver instance.
 * @param base Pointer to the base driver structure.
 * @return CFN_HAL_ERROR_OK on success, or an error code on failure.
 */
CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_unlock(cfn_hal_driver_t *base);
#endif

/* Include Implementation -------------------------------------------*/

#if !defined(CFN_HAL_USE_LIB_BASE) && !defined(CFN_HAL_COMPILE_BASE)
#include "cfn_hal_base_impl.h"
#endif

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_BASE_H
