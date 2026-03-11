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
 * @file test_hal_clock.cpp
 * @brief Hardware Abstraction Layer for TEST_CLOCK.CPP.
 */

#include <gtest/gtest.h>
#include "hal_clock.h"

class ClockTest : public ::testing::Test
{
  protected:
    hal_clock_t     driver{};
    hal_clock_api_t api{};

    void SetUp() override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_CLOCK;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(ClockTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(hal_clock_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(ClockTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_clock_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(ClockTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_clock_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_clock_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(ClockTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ(hal_clock_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(ClockTest, hal_clock_init_Success)
{
    api.hal_clock_init = [](hal_clock_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_clock_init(&driver), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_deinit_Success)
{
    api.hal_clock_deinit = [](hal_clock_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_clock_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_cfg_get_Success)
{
    api.hal_clock_cfg_get = [](hal_clock_t *driver, hal_clock_config_t *config) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    hal_clock_config_t dummy_config{};
    EXPECT_EQ(hal_clock_cfg_get(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_cfg_set_Success)
{
    api.hal_clock_cfg_set = [](hal_clock_t *driver, const hal_clock_config_t *config) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    hal_clock_config_t dummy_config{};
    EXPECT_EQ(hal_clock_cfg_set(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_suspend_tick_Success)
{
    api.hal_clock_suspend_tick = [](hal_clock_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_clock_suspend_tick(&driver), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_resume_tick_Success)
{
    api.hal_clock_resume_tick = [](hal_clock_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_clock_resume_tick(&driver), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_get_system_freq_Success)
{
    api.hal_clock_get_system_freq = [](hal_clock_t *driver, uint32_t *freq_hz) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    uint32_t dummy_freq_hz{};
    EXPECT_EQ(hal_clock_get_system_freq(&driver, &dummy_freq_hz), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_get_peripheral_freq_Success)
{
    api.hal_clock_get_peripheral_freq =
        [](hal_clock_t *driver, uint32_t peripheral_id, uint32_t *freq_hz) -> hal_error_code_t { return HAL_ERROR_OK; };
    uint32_t dummy_freq_hz{};
    EXPECT_EQ(hal_clock_get_peripheral_freq(&driver, 0, &dummy_freq_hz), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_enable_gate_Success)
{
    api.hal_clock_enable_gate = [](hal_clock_t *driver, uint32_t peripheral_id) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_clock_enable_gate(&driver, 0), HAL_ERROR_OK);
}

TEST_F(ClockTest, hal_clock_disable_gate_Success)
{
    api.hal_clock_disable_gate = [](hal_clock_t *driver, uint32_t peripheral_id) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_clock_disable_gate(&driver, 0), HAL_ERROR_OK);
}

TEST_F(ClockTest, LockTimeoutAbortsOperation)
{
    // Mock the lock function to simulate a timeout
    driver.base.lock = [](hal_driver_t *b, uint32_t timeout) -> hal_error_code_t { return HAL_ERROR_TIMING_TIMEOUT; };

    // Attempt the locked operation
    EXPECT_EQ(hal_clock_suspend_tick(&driver), HAL_ERROR_TIMING_TIMEOUT);
}
