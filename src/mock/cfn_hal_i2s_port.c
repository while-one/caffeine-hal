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
 * @file cfn_hal_i2s_port.c
 * @brief Generic I2S HAL Port Template Implementation
 */

#include "cfn_hal_i2s.h"
#include "cfn_hal_i2s_port.h"

static cfn_hal_error_code_t
port_base_event_get (cfn_hal_driver_t *p_base, uint32_t *p_event_mask)
{
    CFN_HAL_UNUSED(p_base);
    if (p_event_mask)
    {
        *p_event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_base_error_get (cfn_hal_driver_t *p_base, uint32_t *p_error_mask)
{
    CFN_HAL_UNUSED(p_base);
    if (p_error_mask)
    {
        *p_error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static const cfn_hal_i2s_api_t I2S_API = {
    .base = {
        .init = NULL,
        .deinit = NULL,
        .power_state_set = NULL,
        .config_set = NULL,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = port_base_event_get,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = port_base_error_get,
    },
    .transmit_dma = NULL,
    .receive_dma = NULL,
    .pause = NULL,
    .resume = NULL,
    .stop = NULL
};

cfn_hal_error_code_t
cfn_hal_i2s_construct (cfn_hal_i2s_t              *p_driver,
                       const cfn_hal_i2s_config_t *p_config,
                       const cfn_hal_i2s_phy_t    *p_phy,
                       struct cfn_hal_clock_s     *p_clock,
                       void                       *p_dependency,
                       cfn_hal_i2s_callback_t      p_callback,
                       void                       *p_user_arg)
{
    if ((p_driver == NULL) || (p_phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_i2s_populate(p_driver, 0, p_clock, p_dependency, &I2S_API, p_phy, p_config, p_callback, p_user_arg);
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t
cfn_hal_i2s_destruct (cfn_hal_i2s_t *p_driver)
{
    if (p_driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_i2s_populate(p_driver, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
