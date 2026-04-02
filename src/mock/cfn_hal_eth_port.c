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

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_eth_start(cfn_hal_eth_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_eth_stop(cfn_hal_eth_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_eth_transmit_frame(cfn_hal_eth_t *driver, const uint8_t *frame, size_t length, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(frame);
    CFN_HAL_UNUSED(length);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_eth_receive_frame(cfn_hal_eth_t *driver,
                                                   uint8_t       *buffer,
                                                   size_t         max_length,
                                                   size_t        *received_length,
                                                   uint32_t       timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(buffer);
    CFN_HAL_UNUSED(max_length);
    CFN_HAL_UNUSED(received_length);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_read_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t *value, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(value);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_write_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t value, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(value);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_eth_get_link_status(cfn_hal_eth_t *driver, cfn_hal_eth_link_status_t *status, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(status);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_OK;
}

static const cfn_hal_eth_api_t eth_api = {
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

cfn_hal_error_code_t cfn_hal_eth_construct(cfn_hal_eth_t              *driver,
                                           const cfn_hal_eth_config_t *config,
                                           const cfn_hal_eth_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           cfn_hal_eth_callback_t      callback,
                                           void                       *user_arg)
{
    if ((driver == NULL) || (phy == NULL)) { return CFN_HAL_ERROR_BAD_PARAM; }
    cfn_hal_eth_populate(driver, 0, clock, &eth_api, phy, config, callback, user_arg);
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_eth_destruct(cfn_hal_eth_t *driver)
{
    if (driver == NULL) { return CFN_HAL_ERROR_BAD_PARAM; }
    cfn_hal_eth_populate(driver, 0, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
