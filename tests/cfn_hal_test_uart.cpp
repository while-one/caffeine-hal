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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_test_uart.cpp
 * @brief Unit tests for UART HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_uart.h"

class UartTest : public ::testing::Test
{
  protected:
    cfn_hal_uart_t     driver{};
    cfn_hal_uart_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_UART;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(UartTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_uart_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_uart_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(UartTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_uart_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_uart_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(UartTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default
    EXPECT_EQ(cfn_hal_uart_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(UartTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_uart_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(UartTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_uart_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(UartTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_uart_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(UartTest, ConfigValidation)
{
    cfn_hal_uart_config_t config = {
        .echo       = false,
        .baudrate   = 115200,
        .read_mode  = CFN_HAL_UART_CONFIG_MODE_BLOCKING,
        .write_mode = CFN_HAL_UART_CONFIG_MODE_BLOCKING,
        .data_len   = CFN_HAL_UART_CONFIG_DATA_LEN_8,
        .stop_bits  = CFN_HAL_UART_CONFIG_STOP_ONE_BIT,
        .parity     = CFN_HAL_UART_CONFIG_PARITY_NONE,
        .flow_ctrl  = CFN_HAL_UART_CONFIG_FLOW_CTRL_NONE,
        .direction  = CFN_HAL_UART_CONFIG_DIRECTION_TX_RX,
        .custom     = nullptr,
    };

    // Valid config
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_OK);

    // NULL driver
    EXPECT_EQ(cfn_hal_uart_config_validate(nullptr, &config), CFN_HAL_ERROR_BAD_PARAM);

    // NULL config
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, nullptr), CFN_HAL_ERROR_BAD_PARAM);

    // Invalid enum (Read Mode)
    config.read_mode = CFN_HAL_UART_CONFIG_MODE_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.read_mode = CFN_HAL_UART_CONFIG_MODE_BLOCKING;

    // Invalid enum (Write Mode)
    config.write_mode = CFN_HAL_UART_CONFIG_MODE_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.write_mode = CFN_HAL_UART_CONFIG_MODE_BLOCKING;

    // Invalid enum (Data Len)
    config.data_len = CFN_HAL_UART_CONFIG_DATA_LEN_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.data_len = CFN_HAL_UART_CONFIG_DATA_LEN_8;

    // Invalid enum (Stop Bits)
    config.stop_bits = CFN_HAL_UART_CONFIG_STOP_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.stop_bits = CFN_HAL_UART_CONFIG_STOP_ONE_BIT;

    // Invalid enum (Parity)
    config.parity = CFN_HAL_UART_CONFIG_PARITY_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.parity = CFN_HAL_UART_CONFIG_PARITY_NONE;

    // Invalid enum (Flow Control)
    config.flow_ctrl = CFN_HAL_UART_CONFIG_FLOW_CTRL_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.flow_ctrl = CFN_HAL_UART_CONFIG_FLOW_CTRL_NONE;

    // Invalid enum (Direction)
    config.direction = CFN_HAL_UART_CONFIG_DIRECTION_MAX;
    EXPECT_EQ(cfn_hal_uart_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
}

TEST_F(UartTest, ConfigSetGet)
{
    cfn_hal_uart_config_t config = { .baudrate = 115200 };
    cfn_hal_uart_config_t read_config{};

    EXPECT_EQ(cfn_hal_uart_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.baudrate, 115200);
}

TEST_F(UartTest, CallbackRegister)
{
    driver.base.status         = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_uart_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(UartTest, TxRxPollingSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.tx_polling     = [](cfn_hal_uart_t *d, const uint8_t *data, size_t len, uint32_t t) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.rx_polling = [](cfn_hal_uart_t *d, uint8_t *data, size_t len, uint32_t t) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    uint8_t buf[4] = { 1, 2, 3, 4 };
    EXPECT_EQ(cfn_hal_uart_tx_polling(&driver, buf, 4, 100), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_rx_polling(&driver, buf, 4, 100), CFN_HAL_ERROR_OK);
}

TEST_F(UartTest, TxRxIrqSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.tx_irq         = [](cfn_hal_uart_t *d, const uint8_t *data, size_t len) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.rx_n_irq = [](cfn_hal_uart_t *d, uint8_t *data, size_t len) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.rx_irq     = [](cfn_hal_uart_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    uint8_t buf[4] = { 1, 2, 3, 4 };
    EXPECT_EQ(cfn_hal_uart_tx_irq(&driver, buf, 4), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_rx_n_irq(&driver, buf, 4), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_rx_irq(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(UartTest, TxRxIrqAbortSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.tx_irq_abort   = [](cfn_hal_uart_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.rx_irq_abort   = [](cfn_hal_uart_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_uart_tx_irq_abort(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_rx_irq_abort(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(UartTest, RxToIdleSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.rx_to_idle =
        [](cfn_hal_uart_t *d, uint8_t *data, size_t max, size_t *received, uint32_t t) -> cfn_hal_error_code_t
    {
        *received = 10;
        return CFN_HAL_ERROR_OK;
    };

    uint8_t buf[32];
    size_t  received = 0;
    EXPECT_EQ(cfn_hal_uart_rx_to_idle(&driver, buf, 32, &received, 100), CFN_HAL_ERROR_OK);
    EXPECT_EQ(received, 10);
}

TEST_F(UartTest, TxRxDmaSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.tx_dma         = [](cfn_hal_uart_t *d, const uint8_t *data, size_t len) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.rx_dma = [](cfn_hal_uart_t *d, uint8_t *data, size_t len) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    uint8_t buf[4] = { 1, 2, 3, 4 };
    EXPECT_EQ(cfn_hal_uart_tx_dma(&driver, buf, 4), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_rx_dma(&driver, buf, 4), CFN_HAL_ERROR_OK);
}

TEST_F(UartTest, EventEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_uart_event_enable(&driver, CFN_HAL_UART_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_event_disable(&driver, CFN_HAL_UART_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(UartTest, ErrorEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_uart_error_enable(&driver, CFN_HAL_UART_ERROR_FRAMING), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_uart_error_disable(&driver, CFN_HAL_UART_ERROR_FRAMING), CFN_HAL_ERROR_OK);
}

TEST_F(UartTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.tx_polling     = [](cfn_hal_uart_t *d, const uint8_t *data, size_t len, uint32_t t) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    uint8_t              buf[1] = { 0 };
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_uart_tx_polling, buf, 1, 100);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
