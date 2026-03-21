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
#include "cfn_hal_types.h"
#include <stddef.h>

/**
 * @def CFN_HAL_USE_LOCK
 * @brief Enable RTOS-aware thread safety for the HAL.
 *
 * If set to 1, the HAL adds a 'lock_obj' opaque pointer to the base driver
 * structure and enables the CFN_HAL_WITH_LOCK macro.
 *
 * @note To use locking, the hardware port implementation MUST:
 * 1. Implement the 'lock' and 'unlock' hooks in the VMT base layer.
 * 2. Assign a valid RTOS mutex or semaphore to the 'lock_obj' member during
 *    the initialization phase (on_config INIT).
 * 3. Handle timeout logic within the 'lock' VMT implementation.
 */
#ifndef CFN_HAL_USE_LOCK
#define CFN_HAL_USE_LOCK 0
#endif

#ifndef CFN_HAL_INLINE
/**
 * @brief Macro for inlining HAL wrapper functions.
 * Can be overridden with __attribute__((always_inline)) for
 * performance-critical ports.
 */
#define CFN_HAL_INLINE static inline
#endif

/* Defines ----------------------------------------------------------*/
#define CFN_HAL_MAX_DELAY (UINT32_MAX)

/* Macro ------------------------------------------------------------*/
/**
 * @brief Cross-compiler static assertion macro.
 * Uses C++11 static_assert or C11 _Static_assert to avoid dependency on
 * <assert.h>.
 */
#ifdef __cplusplus
#define CFN_HAL_STATIC_ASSERT static_assert
#else
#define CFN_HAL_STATIC_ASSERT _Static_assert
#endif

#define CFN_HAL_BIT(x)                ((1UL) << (x))                   /*!< Bit position                          */
#define CFN_HAL_BIT_SET(x, y)         ((x) |= (CFN_HAL_BIT(y)))        /*!< Set bit y of the variable X            */
#define CFN_HAL_BIT_CLEAR(x, y)       ((x) &= ~(CFN_HAL_BIT(y)))       /*!< Clear bit y of the variable x          */
#define CFN_HAL_BIT_TOGGLE(x, y)      ((x) ^= (CFN_HAL_BIT(y)))        /*!< Toggle bit y of the variable x         */
#define CFN_HAL_BIT_CHECK(x, y)       ((x) & (CFN_HAL_BIT(y)))         /*!< Check if bit y is set in variable x    */
#define CFN_HAL_BIT_IS_SET(x, y)      ((CFN_HAL_BIT_CHECK(x, y)) != 0) /*!< Check if bit y is set in variable x */
#define CFN_HAL_STR_STRINGIFY(x)      #x                               /*!< Stringify                              */
#define CFN_HAL_INTERNAL_CONCAT(x, y) x##y                             /*!< Internal concat, not to be used */
#define CFN_HAL_CONCAT(x, y)          CFN_HAL_INTERNAL_CONCAT(x, y)    /*!< Actual concat, can be called           */
#define CFN_HAL_UNUSED(x)             (void) (x)

#define CFN_HAL_CONTAINER_OF(ptr, type, member) ((type *) ((char *) (ptr) - offsetof(type, member)))

/**
 * @brief Helper macro to get the peripheral driver pointer from a base driver
 * pointer.
 * @param ptr Pointer to the base cfn_hal_driver_t structure.
 * @param type Peripheral prefix (e.g. cfn_hal_uart_t, cfn_hal_spi_t,
 * cfn_hal_gpio_t).
 * @return Typed pointer to the peripheral driver structure.
 */
#define CFN_HAL_GET_DRIVER_FROM_BASE(ptr, type) CFN_HAL_CONTAINER_OF(ptr, type, base)

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

#define CFN_HAL_GET_LOCK_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME

#if (CFN_HAL_USE_LOCK == 1)
#define CFN_HAL_LOCK(driver, timeout) cfn_hal_base_lock((cfn_hal_driver_t *) (driver), timeout)
#define CFN_HAL_UNLOCK(driver)        cfn_hal_base_unlock((cfn_hal_driver_t *) (driver))

#define CFN_HAL_WITH_LOCK_0(driver, timeout, result, function)                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((driver) != NULL)                                                                                          \
        {                                                                                                              \
            (result) = cfn_hal_base_lock((cfn_hal_driver_t *) (driver), (timeout));                                    \
            if ((result) == CFN_HAL_ERROR_OK)                                                                          \
            {                                                                                                          \
                (result) = function((driver));                                                                         \
                (void) cfn_hal_base_unlock((cfn_hal_driver_t *) (driver));                                             \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (result) = CFN_HAL_ERROR_BAD_PARAM;                                                                        \
        }                                                                                                              \
    } while (0)

#define CFN_HAL_WITH_LOCK_N(driver, timeout, result, function, ...)                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((driver) != NULL)                                                                                          \
        {                                                                                                              \
            (result) = cfn_hal_base_lock((cfn_hal_driver_t *) (driver), (timeout));                                    \
            if ((result) == CFN_HAL_ERROR_OK)                                                                          \
            {                                                                                                          \
                (result) = function((driver), __VA_ARGS__);                                                            \
                (void) cfn_hal_base_unlock((cfn_hal_driver_t *) (driver));                                             \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (result) = CFN_HAL_ERROR_BAD_PARAM;                                                                        \
        }                                                                                                              \
    } while (0)

/**
 * @brief Helper macro to execute a driver function with a concurrency lock.
 * @param driver Pointer to the peripheral driver instance.
 * @param timeout Lock acquisition timeout in milliseconds.
 * @param result Variable to store the return code (cfn_hal_error_code_t).
 * @param function The driver function to call.
 * @param ... Additional arguments to pass to the function after the driver
 * pointer.
 */
#define CFN_HAL_WITH_LOCK(...)                                                                                         \
    CFN_HAL_GET_LOCK_MACRO(__VA_ARGS__,                                                                                \
                           CFN_HAL_WITH_LOCK_N,                                                                        \
                           CFN_HAL_WITH_LOCK_N,                                                                        \
                           CFN_HAL_WITH_LOCK_N,                                                                        \
                           CFN_HAL_WITH_LOCK_N,                                                                        \
                           CFN_HAL_WITH_LOCK_N,                                                                        \
                           CFN_HAL_WITH_LOCK_0)                                                                        \
    (__VA_ARGS__)
#else
#define CFN_HAL_LOCK(driver, timeout) (CFN_HAL_ERROR_OK)
#define CFN_HAL_UNLOCK(driver)        (CFN_HAL_ERROR_OK)

#define CFN_HAL_WITH_LOCK_NO_LOCK_0(driver, timeout, result, function)                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        (void) (timeout);                                                                                              \
        if ((driver) != NULL)                                                                                          \
        {                                                                                                              \
            (result) = function((driver));                                                                             \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (result) = CFN_HAL_ERROR_BAD_PARAM;                                                                        \
        }                                                                                                              \
    } while (0)

#define CFN_HAL_WITH_LOCK_NO_LOCK_N(driver, timeout, result, function, ...)                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        (void) (timeout);                                                                                              \
        if ((driver) != NULL)                                                                                          \
        {                                                                                                              \
            (result) = function((driver), __VA_ARGS__);                                                                \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (result) = CFN_HAL_ERROR_BAD_PARAM;                                                                        \
        }                                                                                                              \
    } while (0)

/**
 * @brief Helper macro to execute a driver function (Locking disabled).
 */
#define CFN_HAL_WITH_LOCK(...)                                                                                         \
    CFN_HAL_GET_LOCK_MACRO(__VA_ARGS__,                                                                                \
                           CFN_HAL_WITH_LOCK_NO_LOCK_N,                                                                \
                           CFN_HAL_WITH_LOCK_NO_LOCK_N,                                                                \
                           CFN_HAL_WITH_LOCK_NO_LOCK_N,                                                                \
                           CFN_HAL_WITH_LOCK_NO_LOCK_N,                                                                \
                           CFN_HAL_WITH_LOCK_NO_LOCK_N,                                                                \
                           CFN_HAL_WITH_LOCK_NO_LOCK_0)                                                                \
    (__VA_ARGS__)
#endif

/* Types Enums ------------------------------------------------------*/
/* Types Structs ----------------------------------------------------*/
/* Functions prototypes ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_H
