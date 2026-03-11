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
 * @file test_hal_adc.cpp
 * @brief Hardware Abstraction Layer for TEST_ADC.CPP.
 */

#include <gtest/gtest.h>
#include "hal_adc.h"

class AdcTest : public ::testing::Test
{
  protected:
    hal_adc_t     driver{};
    hal_adc_api_t api{};

    void SetUp() override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_ADC;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(AdcTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(hal_adc_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(AdcTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_adc_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(AdcTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_adc_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_adc_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(AdcTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ(hal_adc_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(AdcTest, hal_adc_init_Success)
{
    api.hal_adc_init = [](hal_adc_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_init(&driver), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_deinit_Success)
{
    api.hal_adc_deinit = [](hal_adc_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_read_Success)
{
    api.hal_adc_read = [](hal_adc_t *driver, uint32_t *value, uint32_t timeout) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    uint32_t dummy_value{};
    EXPECT_EQ(hal_adc_read(&driver, &dummy_value, 0), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_start_Success)
{
    api.hal_adc_start = [](hal_adc_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_start(&driver), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_stop_Success)
{
    api.hal_adc_stop = [](hal_adc_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_stop(&driver), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_register_cb_Success)
{
    api.hal_adc_register_cb = [](hal_adc_t *driver, hal_adc_callback_t cb, void *user_arg) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_register_cb(&driver, (hal_adc_callback_t) nullptr, nullptr), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_cfg_irq_enable_Success)
{
    api.hal_adc_cfg_irq_enable = [](hal_adc_t *driver, hal_adc_interrupts_t irq) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_cfg_irq_enable(&driver, (hal_adc_interrupts_t) 0), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_cfg_irq_disable_Success)
{
    api.hal_adc_cfg_irq_disable = [](hal_adc_t *driver, hal_adc_interrupts_t irq) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_adc_cfg_irq_disable(&driver, (hal_adc_interrupts_t) 0), HAL_ERROR_OK);
}

TEST_F(AdcTest, hal_adc_read_dma_Success)
{
    api.hal_adc_read_dma = [](hal_adc_t *driver, uint32_t *data, size_t nbr_of_samples) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    uint32_t dummy_data{};
    EXPECT_EQ(hal_adc_read_dma(&driver, &dummy_data, 0), HAL_ERROR_OK);
}

TEST_F(AdcTest, LockTimeoutAbortsOperation)
{
    // Mock the lock function to simulate a timeout
    driver.base.lock = [](hal_driver_t *b, uint32_t timeout) -> hal_error_code_t { return HAL_ERROR_TIMING_TIMEOUT; };
    uint32_t dummy_value{};

    // Attempt the locked operation
    EXPECT_EQ(hal_adc_read(&driver, &dummy_value, 0), HAL_ERROR_TIMING_TIMEOUT);
}
