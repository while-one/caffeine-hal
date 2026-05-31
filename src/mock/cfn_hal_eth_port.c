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
 * @file cfn_hal_eth_port.c
 * @brief Generic ETH HAL Port Template Implementation
 */

#include "cfn_hal_eth.h"
#include "cfn_hal_eth_port.h"

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

static cfn_hal_error_code_t
port_eth_start (cfn_hal_eth_t *p_driver)
{
    CFN_HAL_UNUSED(p_driver);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_stop (cfn_hal_eth_t *p_driver)
{
    CFN_HAL_UNUSED(p_driver);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_transmit_frame (cfn_hal_eth_t *p_driver, const uint8_t *p_frame, size_t length, uint32_t timeout)
{
    CFN_HAL_UNUSED(p_driver);
    CFN_HAL_UNUSED(p_frame);
    CFN_HAL_UNUSED(length);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_receive_frame (cfn_hal_eth_t *p_driver,
                        uint8_t       *p_buffer, // NOLINT(readability-non-const-parameter)
                        size_t         max_length,
                        size_t        *p_received_length, // NOLINT(readability-non-const-parameter)
                        uint32_t       timeout)           // NOLINT(readability-non-const-parameter)
{
    CFN_HAL_UNUSED(p_driver);
    CFN_HAL_UNUSED(p_buffer);
    CFN_HAL_UNUSED(max_length);
    CFN_HAL_UNUSED(p_received_length);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_read_phy_reg (cfn_hal_eth_t *p_driver,
                       uint16_t       phy_addr,
                       uint16_t       reg_addr,
                       uint16_t      *p_value, // NOLINT(readability-non-const-parameter)
                       uint32_t       timeout) // NOLINT(readability-non-const-parameter)
{
    CFN_HAL_UNUSED(p_driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(p_value);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_write_phy_reg (cfn_hal_eth_t *p_driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t value, uint32_t timeout)
{
    CFN_HAL_UNUSED(p_driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(value);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_get_link_status (cfn_hal_eth_t *p_driver, cfn_hal_eth_link_status_t *p_status, uint32_t timeout)
{
    CFN_HAL_UNUSED(p_driver);
    CFN_HAL_UNUSED(p_status);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static const cfn_hal_eth_api_t ETH_API = {
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
    .start = port_eth_start,
    .stop = port_eth_stop,
    .transmit_frame = port_eth_transmit_frame,
    .receive_frame = port_eth_receive_frame,
    .read_phy_reg = port_eth_read_phy_reg,
    .write_phy_reg = port_eth_write_phy_reg,
    .get_link_status = port_eth_get_link_status
};

cfn_hal_error_code_t
cfn_hal_eth_construct (cfn_hal_eth_t              *p_driver,
                       const cfn_hal_eth_config_t *p_config,
                       const cfn_hal_eth_phy_t    *p_phy,
                       struct cfn_hal_clock_s     *p_clock,
                       void                       *p_dependency,
                       cfn_hal_eth_callback_t      p_callback,
                       void                       *p_user_arg)
{
    if ((p_driver == NULL) || (p_phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_eth_populate(p_driver, 0, p_clock, p_dependency, &ETH_API, p_phy, p_config, p_callback, p_user_arg);
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t
cfn_hal_eth_destruct (cfn_hal_eth_t *p_driver)
{
    if (p_driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_eth_populate(p_driver, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
