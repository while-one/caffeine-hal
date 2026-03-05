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
 * @file hal_timer.h
 * @brief Hardware Timer HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_TIMER_H
#define CAFFEINE_HAL_HAL_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_TIMER_INTERRUPT_TRIGGER,
    HAL_TIMER_INTERRUPT_CC,

    HAL_TIMER_INTERRUPT_ALARM,
    HAL_TIMER_INTERRUPT_UPDATE,
    HAL_TIMER_INTERRUPT_ERROR,
} hal_timer_interrupts_t;

typedef enum
{
    HAL_TIMER_CFG_COUNTER_MODE_UP,
    HAL_TIMER_CFG_COUNTER_MODE_DOWN
} hal_timer_cfg_counter_mode_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    hal_timer_cfg_counter_mode_t mode;
    uint32_t                     period;
    bool                         auto_reload;
    void                        *user_arg;
} hal_timer_config_t;

typedef struct
{
    void        *port;
    uint32_t     channel;
    hal_driver_t gpio;
} hal_timer_phy_t;

typedef struct hal_timer_s hal_timer_t;

typedef struct hal_timer_api_s hal_timer_api_t;

typedef void (*hal_timer_callback_t) (hal_timer_t *driver, hal_timer_interrupts_t interrupt, uint32_t channel,
                                      void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_timer_api_s
{
    hal_error_code_t (*hal_timer_start) (hal_timer_t *driver);
    hal_error_code_t (*hal_timer_stop) (hal_timer_t *driver);
    hal_error_code_t (*hal_timer_get_ticks) (hal_timer_t *driver, uint32_t ch, uint32_t *ticks);
    hal_error_code_t (*hal_timer_get_ticks_u64) (hal_timer_t *driver, uint32_t ch, uint64_t *ticks);
    hal_error_code_t (*hal_timer_set_period) (hal_timer_t *driver, uint32_t ch, uint64_t ticks);
    hal_error_code_t (*hal_timer_init) (hal_timer_t *driver);
    hal_error_code_t (*hal_timer_deinit) (hal_timer_t *driver);
    hal_error_code_t (*hal_timer_cfg_get) (hal_timer_t *driver, hal_timer_config_t *config);
    hal_error_code_t (*hal_timer_cfg_set) (hal_timer_t *driver, const hal_timer_config_t *config);
    hal_error_code_t (*hal_timer_cfg_irq_enable) (hal_timer_t *driver, hal_timer_interrupts_t irq);
    hal_error_code_t (*hal_timer_cfg_irq_disable) (hal_timer_t *driver, hal_timer_interrupts_t irq);
};

HAL_CREATE_DRIVER_TYPE (timer, hal_timer_config_t, hal_timer_api_t, hal_timer_phy_t, hal_timer_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_timer_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_start (hal_timer_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_start, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_timer_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_stop (hal_timer_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_stop, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_timer_get_ticks implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param ch Channel number.
 * @param ticks Timer ticks value.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_get_ticks (hal_timer_t *driver, uint32_t ch, uint32_t *ticks)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_get_ticks, driver, error, ch, ticks);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_timer_get_ticks_u64 implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param ch Channel number.
 * @param ticks Timer ticks value.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_get_ticks_u64 (hal_timer_t *driver, uint32_t ch, uint64_t *ticks)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_get_ticks_u64, driver, error, ch, ticks);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_timer_set_period implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param ch Channel number.
 * @param ticks Timer ticks value.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_set_period (hal_timer_t *driver, uint32_t ch, uint64_t ticks)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_set_period, driver, error, ch, ticks);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_timer_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_init (hal_timer_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_TIMER;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_init, driver, error);
    return error;
}

/**
 * @brief hal_timer_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_deinit (hal_timer_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_timer_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_cfg_get (hal_timer_t *driver, hal_timer_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_timer_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_cfg_set (hal_timer_t *driver, const hal_timer_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_timer_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_cfg_irq_enable (hal_timer_t *driver, hal_timer_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_timer_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_timer_cfg_irq_disable (hal_timer_t *driver, hal_timer_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_TIMER, hal_timer_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TIMER_H
