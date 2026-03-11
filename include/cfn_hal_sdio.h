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

#include "cfn_hal_types.h"
#include "cfn_hal.h"

typedef struct
{
    void *user_config;
} cfn_hal_sdio_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_sdio_phy_t;

typedef struct cfn_hal_sdio_s     cfn_hal_sdio_t;
typedef struct cfn_hal_sdio_api_s cfn_hal_sdio_api_t;

struct cfn_hal_sdio_api_s
{
    cfn_hal_error_code_t (*cfn_hal_sdio_init)(cfn_hal_sdio_t *driver);
    cfn_hal_error_code_t (*cfn_hal_sdio_deinit)(cfn_hal_sdio_t *driver);
};

/**
 * @brief Generated driver structure for sdio.
 * This macro expands to define `struct cfn_hal_sdio_s` and the typedef `cfn_hal_sdio_t`.
 */
CFN_HAL_CREATE_DRIVER_TYPE(sdio, cfn_hal_sdio_config_t, cfn_hal_sdio_api_t, cfn_hal_sdio_phy_t, void *);

static inline cfn_hal_error_code_t cfn_hal_sdio_init(cfn_hal_sdio_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    if (driver && driver->base.on_config)
    {
        error = driver->base.on_config(&driver->base, DRIVER_CONFIG_INIT);
        if (error != CFN_HAL_ERROR_OK)
        {
            return error;
        }
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SDIO, cfn_hal_sdio_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_SDIO_H
