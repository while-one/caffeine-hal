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
 * @file hal_irq.h
 * @brief Generic Interrupt Controller HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_IRQ_H
#define CAFFEINE_HAL_HAL_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_types.h"
#include "hal.h"

typedef struct
{
    void *user_config;
} hal_irq_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} hal_irq_phy_t;

typedef struct hal_irq_s     hal_irq_t;
typedef struct hal_irq_api_s hal_irq_api_t;

struct hal_irq_api_s
{
    hal_error_code_t (*hal_irq_init) (hal_irq_t *driver);
    hal_error_code_t (*hal_irq_deinit) (hal_irq_t *driver);
    hal_error_code_t (*hal_irq_global_enable) (hal_irq_t *driver);
    hal_error_code_t (*hal_irq_global_disable) (hal_irq_t *driver);
    hal_error_code_t (*hal_irq_enable_vector) (hal_irq_t *driver, uint32_t irq_id);
    hal_error_code_t (*hal_irq_disable_vector) (hal_irq_t *driver, uint32_t irq_id);
    hal_error_code_t (*hal_irq_set_priority) (hal_irq_t *driver, uint32_t irq_id, uint32_t priority);
    hal_error_code_t (*hal_irq_clear_pending) (hal_irq_t *driver, uint32_t irq_id);
};

HAL_CREATE_DRIVER_TYPE (irq, hal_irq_config_t, hal_irq_api_t, hal_irq_phy_t, void *);

static inline hal_error_code_t hal_irq_init (hal_irq_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_IRQ;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_init, driver, error);
    return error;
}

static inline hal_error_code_t hal_irq_deinit (hal_irq_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

static inline hal_error_code_t hal_irq_global_enable (hal_irq_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_global_enable, driver, error);
    return error;
}

static inline hal_error_code_t hal_irq_global_disable (hal_irq_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_global_disable, driver, error);
    return error;
}

static inline hal_error_code_t hal_irq_enable_vector (hal_irq_t *driver, uint32_t irq_id)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_enable_vector, driver, error, irq_id);
    return error;
}

static inline hal_error_code_t hal_irq_disable_vector (hal_irq_t *driver, uint32_t irq_id)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_disable_vector, driver, error, irq_id);
    return error;
}

static inline hal_error_code_t hal_irq_set_priority (hal_irq_t *driver, uint32_t irq_id, uint32_t priority)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_set_priority, driver, error, irq_id, priority);
    return error;
}

static inline hal_error_code_t hal_irq_clear_pending (hal_irq_t *driver, uint32_t irq_id)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_IRQ, hal_irq_clear_pending, driver, error, irq_id);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_IRQ_H
