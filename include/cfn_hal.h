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
 * @file cfn_hal.h
 * @brief Core Hardware Abstraction Layer definitions and macros.
 */

#ifndef CAFFEINE_HAL_HAL_H
#define CAFFEINE_HAL_HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <stddef.h>
#include "cfn_hal_types.h"

/* Defines ----------------------------------------------------------*/
// #define CFN_HAL_CAFFEINE_VERSION CAFFEINE_HAL_VERSION /*!< Managed by the build system */
#define CFN_HAL_MAX_DELAY (UINT32_MAX)

/* Macro ------------------------------------------------------------*/
#define CFN_HAL_BIT(x)                (1UL << (x))                     /*!< Bit position                          */
#define CFN_HAL_BIT_SET(x, y)         ((x) |= (CFN_HAL_BIT(y)))        /*!< Set bit y of the variable X            */
#define CFN_HAL_BIT_CLEAR(x, y)       ((x) &= ~(CFN_HAL_BIT(y)))       /*!< Clear bit y of the variable x          */
#define CFN_HAL_BIT_TOGGLE(x, y)      ((x) ^= (CFN_HAL_BIT(y)))        /*!< Toggle bit y of the variable x         */
#define CFN_HAL_BIT_CHECK(x, y)       ((x) & (CFN_HAL_BIT(y)))         /*!< Check if bit y is set in variable x    */
#define CFN_HAL_BIT_IS_SET(x, y)      ((CFN_HAL_BIT_CHECK(x, y)) != 0) /*!< Check if bit y is set in variable x */
#define CFN_HAL_STR_STRINGIFY(x)      #x                               /*!< Stringify                              */
#define CFN_HAL_INTERNAL_CONCAT(x, y) x##y                             /*!< Internal concat, not to be used */
#define CFN_HAL_CONCAT(x, y)          CFN_HAL_INTERNAL_CONCAT(x, y)    /*!< Actual concat, can be called           */
#define CFN_HAL_UNUSED(x)             (void) (x)

/**
 * @brief Core Hardware Abstraction Layer definitions and macros.
 *
 * This file defines the standard interface for the Core Hardware Abstraction
 * Layer definitions and macros..
 */
#define CFN_HAL_CONTAINER_OF(ptr, type, member) ((type *) ((char *) (ptr) - offsetof(type, member)))

/**
 * @brief Helper macro to get the peripheral driver pointer from a base driver
 * pointer.
 * @param ptr Pointer to the base cfn_hal_driver_t structure.
 * @param type Peripheral prefix (e.g. cfn_hal_uart_t, cfn_hal_spi_t, cfn_hal_gpio_t).
 * @return Typed pointer to the peripheral driver structure.
 */
#define CFN_HAL_GET_DRIVER(ptr, type) CFN_HAL_CONTAINER_OF(ptr, type, base)
/**
 * @brief Core Hardware Abstraction Layer definitions and macros.
 *
 * This file defines the standard interface for the Core Hardware Abstraction
 * Layer definitions and macros..
 */
#define CFN_HAL_CREATE_DRIVER_TYPE(prefix, config_type, api_type, phy_type, cb_type)                                   \
    struct cfn_hal_##prefix##_s                                                                                        \
    {                                                                                                                  \
        cfn_hal_driver_t   base;                                                                                       \
        const config_type *config;                                                                                     \
        const api_type    *api;                                                                                        \
        const phy_type    *phy;                                                                                        \
        cb_type            cb;                                                                                         \
        void              *cb_user_arg;                                                                                \
    };                                                                                                                 \
    typedef struct cfn_hal_##prefix##_s cfn_hal_##prefix##_t

#define CFN_HAL_CHECK_AND_CALL_FUNC(expected_peripheral_type, func, driver, result)                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((driver) && (driver)->base.type == (expected_peripheral_type) && (driver)->api)                            \
        {                                                                                                              \
            if ((driver)->api->func)                                                                                   \
            {                                                                                                          \
                (result) = (driver)->api->func((driver));                                                              \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                (result) = CFN_HAL_ERROR_NOT_SUPPORTED;                                                                \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (result) = CFN_HAL_ERROR_BAD_PARAM;                                                                        \
        }                                                                                                              \
    } while (0)

#define CFN_HAL_CHECK_AND_CALL_FUNC_VARG(expected_peripheral_type, func, driver, result, ...)                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((driver) && (driver)->base.type == (expected_peripheral_type) && (driver)->api)                            \
        {                                                                                                              \
            if ((driver)->api->func)                                                                                   \
            {                                                                                                          \
                (result) = (driver)->api->func((driver), __VA_ARGS__);                                                 \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                (result) = CFN_HAL_ERROR_NOT_SUPPORTED;                                                                \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (result) = CFN_HAL_ERROR_BAD_PARAM;                                                                        \
        }                                                                                                              \
    } while (0)
/* Types Enums ------------------------------------------------------*/
/* Types Structs ----------------------------------------------------*/
/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Generic Power Management state transition function.
 * @param base Pointer to the base driver struct (use &driver->base).
 * @param state The target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_pm_set_state(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    if (!base)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->power_state == state)
    {
        return CFN_HAL_ERROR_OK;
    }

    if (!base->set_power_state)
    {
        return CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    cfn_hal_error_code_t err = base->set_power_state(base, state);
    if (err == CFN_HAL_ERROR_OK)
    {
        base->power_state = state;
    }

    return err;
}

/**
 * @brief Helper macro to easily set the power state of any specific driver.
 * Usage: CFN_HAL_PM_SET_POWER(&my_gpio, CFN_HAL_POWER_STATE_OFF);
 */
#define CFN_HAL_PM_SET_POWER(driver_ptr, state) cfn_hal_pm_set_state(&(driver_ptr)->base, state)

/**
 * @brief Generic Concurrency Lock.
 * @param base Pointer to the base driver struct.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, CFN_HAL_ERROR_TIMING_TIMEOUT on timeout, or
 * OK instantly if no lock provided.
 */
static inline cfn_hal_error_code_t cfn_hal_driver_lock(cfn_hal_driver_t *base, uint32_t timeout)
{
    if (!base)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    if (!base->lock)
    {
        return CFN_HAL_ERROR_OK; /* Safe bare-metal fallback */
    }
    return base->lock(base, timeout);
}

/**
 * @brief Generic Concurrency Unlock.
 * @param base Pointer to the base driver struct.
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t cfn_hal_driver_unlock(cfn_hal_driver_t *base)
{
    if (!base)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    if (!base->unlock)
    {
        return CFN_HAL_ERROR_OK; /* Safe bare-metal fallback */
    }
    return base->unlock(base);
}

#ifndef CFN_HAL_USE_LOCK
#define CFN_HAL_USE_LOCK 1
#endif

#if (CFN_HAL_USE_LOCK == 1)
#define CFN_HAL_LOCK(driver_ptr, timeout) cfn_hal_driver_lock(&(driver_ptr)->base, timeout)
#define CFN_HAL_UNLOCK(driver_ptr)        cfn_hal_driver_unlock(&(driver_ptr)->base)
#else
#define CFN_HAL_LOCK(driver_ptr, timeout) (CFN_HAL_ERROR_OK)
#define CFN_HAL_UNLOCK(driver_ptr)        (CFN_HAL_ERROR_OK)
#endif

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_H
