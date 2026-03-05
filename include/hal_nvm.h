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
 * @file hal_nvm.h
 * @brief NVM (Non-Volatile Memory) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_NVM_H
#define CAFFEINE_HAL_HAL_NVM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_types.h"
#include "hal.h"

typedef struct
{
    void *user_config;
} hal_nvm_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} hal_nvm_phy_t;

typedef struct hal_nvm_s     hal_nvm_t;
typedef struct hal_nvm_api_s hal_nvm_api_t;

struct hal_nvm_api_s
{
    hal_error_code_t (*hal_nvm_init) (hal_nvm_t *driver);
    hal_error_code_t (*hal_nvm_deinit) (hal_nvm_t *driver);
};

HAL_CREATE_DRIVER_TYPE (nvm, hal_nvm_config_t, hal_nvm_api_t, hal_nvm_phy_t, void *);

static inline hal_error_code_t hal_nvm_init (hal_nvm_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_NVM;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_NVM, hal_nvm_init, driver, error);
    return error;
}

static inline hal_error_code_t hal_nvm_deinit (hal_nvm_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_NVM, hal_nvm_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_NVM_H
