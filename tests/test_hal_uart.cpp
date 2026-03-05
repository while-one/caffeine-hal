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
 * @file test_hal_uart.cpp
 * @brief Hardware Abstraction Layer for TEST_UART.CPP.
 */

#include <gtest/gtest.h>
#include "hal_uart.h"

class UartTest : public ::testing::Test {
protected:
    hal_uart_t driver{};
    hal_uart_api_t api{};

    void SetUp() override {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_UART;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(UartTest, NullDriverReturnsBadParam) {
    EXPECT_EQ(hal_uart_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(UartTest, WrongPeripheralTypeReturnsBadParam) {
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_uart_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(UartTest, UnimplementedApiReturnsNotSupported) {
    api.hal_uart_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_uart_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(UartTest, OnConfigFailureAbortsInit) {
    driver.base.on_config = [](hal_driver_t* b, bool init) -> hal_error_code_t {
        return HAL_ERROR_FAIL;
    };
    EXPECT_EQ(hal_uart_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(UartTest, hal_uart_register_cb_Success) {
    api.hal_uart_register_cb = [](hal_uart_t *driver, hal_uart_callback_t cb, void *user_arg) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_register_cb(&driver, (hal_uart_callback_t)nullptr, nullptr), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_set_cb_arg_Success) {
    api.hal_uart_set_cb_arg = [](hal_uart_t *driver, void *user_arg) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_set_cb_arg(&driver, nullptr), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_tx_irq_Success) {
    api.hal_uart_tx_irq = [](hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    EXPECT_EQ(hal_uart_tx_irq(&driver, &dummy_data, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_tx_irq_abort_Success) {
    api.hal_uart_tx_irq_abort = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_tx_irq_abort(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_rx_irq_Success) {
    api.hal_uart_rx_irq = [](hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    EXPECT_EQ(hal_uart_rx_irq(&driver, &dummy_data, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_rx_irq_abort_Success) {
    api.hal_uart_rx_irq_abort = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_rx_irq_abort(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_tx_polling_Success) {
    api.hal_uart_tx_polling = [](hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    EXPECT_EQ(hal_uart_tx_polling(&driver, &dummy_data, 0, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_rx_polling_Success) {
    api.hal_uart_rx_polling = [](hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    EXPECT_EQ(hal_uart_rx_polling(&driver, &dummy_data, 0, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_rx_to_idle_Success) {
    api.hal_uart_rx_to_idle = [](hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    size_t dummy_received_bytes{};
    EXPECT_EQ(hal_uart_rx_to_idle(&driver, &dummy_data, 0, &dummy_received_bytes, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_check_error_Success) {
    api.hal_uart_check_error = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_check_error(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_init_Success) {
    api.hal_uart_init = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_init(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_deinit_Success) {
    api.hal_uart_deinit = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_get_Success) {
    api.hal_uart_cfg_get = [](hal_uart_t *driver, hal_uart_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_uart_config_t dummy_config{};
    EXPECT_EQ(hal_uart_cfg_get(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_set_Success) {
    api.hal_uart_cfg_set = [](hal_uart_t *driver, const hal_uart_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_uart_config_t dummy_config{};
    EXPECT_EQ(hal_uart_cfg_set(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_tx_enable_Success) {
    api.hal_uart_cfg_tx_enable = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_cfg_tx_enable(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_rx_enable_Success) {
    api.hal_uart_cfg_rx_enable = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_cfg_rx_enable(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_irq_enable_Success) {
    api.hal_uart_cfg_irq_enable = [](hal_uart_t *driver, hal_uart_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_cfg_irq_enable(&driver, (hal_uart_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_tx_disable_Success) {
    api.hal_uart_cfg_tx_disable = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_cfg_tx_disable(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_rx_disable_Success) {
    api.hal_uart_cfg_rx_disable = [](hal_uart_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_cfg_rx_disable(&driver), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_cfg_irq_disable_Success) {
    api.hal_uart_cfg_irq_disable = [](hal_uart_t *driver, hal_uart_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_uart_cfg_irq_disable(&driver, (hal_uart_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_tx_dma_Success) {
    api.hal_uart_tx_dma = [](hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    EXPECT_EQ(hal_uart_tx_dma(&driver, &dummy_data, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, hal_uart_rx_dma_Success) {
    api.hal_uart_rx_dma = [](hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint8_t dummy_data{};
    EXPECT_EQ(hal_uart_rx_dma(&driver, &dummy_data, 0), HAL_ERROR_OK);
}

TEST_F(UartTest, LockTimeoutAbortsOperation) {
    // Mock the lock function to simulate a timeout
    driver.base.lock = [](hal_driver_t* b, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_TIMING_TIMEOUT;
    };
    uint8_t dummy_data{};
    
    // Attempt the locked operation
    EXPECT_EQ(hal_uart_tx_irq(&driver, &dummy_data, 0), HAL_ERROR_TIMING_TIMEOUT);
}
