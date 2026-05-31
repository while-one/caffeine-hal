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
 * @file cfn_hal_uart_port.c
 * @brief Generic UART HAL Port Template Implementation
 */

#include "cfn_hal_uart.h"
#include "cfn_hal_uart_port.h"

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
port_uart_rx_n_irq (cfn_hal_uart_t *p_driver,
                    uint8_t        *p_data,       // NOLINT(readability-non-const-parameter)
                    size_t          nbr_of_bytes) // NOLINT(readability-non-const-parameter)
{
    if (p_driver)
    {
        p_driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    CFN_HAL_UNUSED(p_driver);
    CFN_HAL_UNUSED(p_data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t
port_uart_rx_irq (cfn_hal_uart_t *p_driver)
{
    if (p_driver)
    {
        p_driver->base.flags |= CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    CFN_HAL_UNUSED(p_driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static const cfn_hal_uart_api_t  UART_API= {
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
    .tx_irq = NULL,
    .tx_irq_abort = NULL,
    .rx_n_irq = port_uart_rx_n_irq,
    .rx_irq = port_uart_rx_irq,
    .rx_irq_abort = NULL,
    .rx_polling = NULL,
    .tx_dma = NULL,
    .rx_dma = NULL
};

cfn_hal_error_code_t
cfn_hal_uart_construct (cfn_hal_uart_t              *p_driver,
                        const cfn_hal_uart_config_t *p_config,
                        const cfn_hal_uart_phy_t    *p_phy,
                        struct cfn_hal_clock_s      *p_clock,
                        void                        *p_dependency,
                        cfn_hal_uart_callback_t      p_callback,
                        void                        *p_user_arg)
{
    if ((p_driver == NULL) || (p_phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_uart_populate(p_driver, 0, p_clock, p_dependency, &UART_API, p_phy, p_config, p_callback, p_user_arg);
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t
cfn_hal_uart_destruct (cfn_hal_uart_t *p_driver)
{
    if (p_driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_uart_populate(p_driver, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
