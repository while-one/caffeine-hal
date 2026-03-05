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
 * @file test_hal_spi.cpp
 * @brief Hardware Abstraction Layer for TEST_SPI.CPP.
 */

#include <gtest/gtest.h>
#include "hal_spi.h"

class SpiTest : public ::testing::Test {
protected:
    hal_spi_t driver{};
    hal_spi_api_t api{};

    void SetUp() override {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_SPI;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(SpiTest, NullDriverReturnsBadParam) {
    EXPECT_EQ(hal_spi_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(SpiTest, WrongPeripheralTypeReturnsBadParam) {
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_spi_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(SpiTest, UnimplementedApiReturnsNotSupported) {
    api.hal_spi_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_spi_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(SpiTest, OnConfigFailureAbortsInit) {
    driver.base.on_config = [](hal_driver_t* b, bool init) -> hal_error_code_t {
        return HAL_ERROR_FAIL;
    };
    EXPECT_EQ(hal_spi_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(SpiTest, hal_spi_register_cb_Success) {
    api.hal_spi_register_cb = [](hal_spi_t *driver, hal_spi_callback_t cb, void *user_arg) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_register_cb(&driver, (hal_spi_callback_t)nullptr, nullptr), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_set_cb_arg_Success) {
    api.hal_spi_set_cb_arg = [](hal_spi_t *driver, void *user_arg) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_set_cb_arg(&driver, nullptr), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_xfr_irq_Success) {
    api.hal_spi_xfr_irq = [](hal_spi_t *driver, hal_spi_transaction_t *xfr) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_spi_transaction_t dummy_xfr{};
    EXPECT_EQ(hal_spi_xfr_irq(&driver, &dummy_xfr), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_xfr_irq_abort_Success) {
    api.hal_spi_xfr_irq_abort = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_xfr_irq_abort(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_xfr_polling_Success) {
    api.hal_spi_xfr_polling = [](hal_spi_t *driver, hal_spi_transaction_t *xfr, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_spi_transaction_t dummy_xfr{};
    EXPECT_EQ(hal_spi_xfr_polling(&driver, &dummy_xfr, 0), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_check_error_Success) {
    api.hal_spi_check_error = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_check_error(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_init_Success) {
    api.hal_spi_init = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_init(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_deinit_Success) {
    api.hal_spi_deinit = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_get_Success) {
    api.hal_spi_cfg_get = [](hal_spi_t *driver, hal_spi_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_spi_config_t dummy_config{};
    EXPECT_EQ(hal_spi_cfg_get(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_set_Success) {
    api.hal_spi_cfg_set = [](hal_spi_t *driver, const hal_spi_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_spi_config_t dummy_config{};
    EXPECT_EQ(hal_spi_cfg_set(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_tx_enable_Success) {
    api.hal_spi_cfg_tx_enable = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_cfg_tx_enable(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_rx_enable_Success) {
    api.hal_spi_cfg_rx_enable = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_cfg_rx_enable(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_irq_enable_Success) {
    api.hal_spi_cfg_irq_enable = [](hal_spi_t *driver, hal_spi_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_cfg_irq_enable(&driver, (hal_spi_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_tx_disable_Success) {
    api.hal_spi_cfg_tx_disable = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_cfg_tx_disable(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_rx_disable_Success) {
    api.hal_spi_cfg_rx_disable = [](hal_spi_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_cfg_rx_disable(&driver), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_cfg_irq_disable_Success) {
    api.hal_spi_cfg_irq_disable = [](hal_spi_t *driver, hal_spi_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_spi_cfg_irq_disable(&driver, (hal_spi_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(SpiTest, hal_spi_xfr_dma_Success) {
    api.hal_spi_xfr_dma = [](hal_spi_t *driver, hal_spi_transaction_t *xfr) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_spi_transaction_t dummy_xfr{};
    EXPECT_EQ(hal_spi_xfr_dma(&driver, &dummy_xfr), HAL_ERROR_OK);
}

TEST_F(SpiTest, LockTimeoutAbortsOperation) {
    // Mock the lock function to simulate a timeout
    driver.base.lock = [](hal_driver_t* b, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_TIMING_TIMEOUT;
    };
    hal_spi_transaction_t dummy_xfr{};
    
    // Attempt the locked operation
    EXPECT_EQ(hal_spi_xfr_irq(&driver, &dummy_xfr), HAL_ERROR_TIMING_TIMEOUT);
}
