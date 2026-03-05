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
 * @file test_hal_timer.cpp
 * @brief Hardware Abstraction Layer for TEST_TIMER.CPP.
 */

#include <gtest/gtest.h>
#include "hal_timer.h"

class TimerTest : public ::testing::Test {
protected:
    hal_timer_t driver{};
    hal_timer_api_t api{};

    void SetUp() override {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_TIMER;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(TimerTest, NullDriverReturnsBadParam) {
    EXPECT_EQ(hal_timer_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(TimerTest, WrongPeripheralTypeReturnsBadParam) {
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_timer_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(TimerTest, UnimplementedApiReturnsNotSupported) {
    api.hal_timer_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_timer_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(TimerTest, OnConfigFailureAbortsInit) {
    driver.base.on_config = [](hal_driver_t* b, bool init) -> hal_error_code_t {
        return HAL_ERROR_FAIL;
    };
    EXPECT_EQ(hal_timer_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(TimerTest, hal_timer_start_Success) {
    api.hal_timer_start = [](hal_timer_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_start(&driver), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_stop_Success) {
    api.hal_timer_stop = [](hal_timer_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_stop(&driver), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_get_ticks_Success) {
    api.hal_timer_get_ticks = [](hal_timer_t *driver, uint32_t ch, uint32_t *ticks) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint32_t dummy_ticks{};
    EXPECT_EQ(hal_timer_get_ticks(&driver, 0, &dummy_ticks), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_get_ticks_u64_Success) {
    api.hal_timer_get_ticks_u64 = [](hal_timer_t *driver, uint32_t ch, uint64_t *ticks) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    uint64_t dummy_ticks{};
    EXPECT_EQ(hal_timer_get_ticks_u64(&driver, 0, &dummy_ticks), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_set_period_Success) {
    api.hal_timer_set_period = [](hal_timer_t *driver, uint32_t ch, uint64_t ticks) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_set_period(&driver, 0, 0), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_init_Success) {
    api.hal_timer_init = [](hal_timer_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_init(&driver), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_deinit_Success) {
    api.hal_timer_deinit = [](hal_timer_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_cfg_get_Success) {
    api.hal_timer_cfg_get = [](hal_timer_t *driver, hal_timer_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_timer_config_t dummy_config{};
    EXPECT_EQ(hal_timer_cfg_get(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_cfg_set_Success) {
    api.hal_timer_cfg_set = [](hal_timer_t *driver, const hal_timer_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_timer_config_t dummy_config{};
    EXPECT_EQ(hal_timer_cfg_set(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_cfg_irq_enable_Success) {
    api.hal_timer_cfg_irq_enable = [](hal_timer_t *driver, hal_timer_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_cfg_irq_enable(&driver, (hal_timer_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(TimerTest, hal_timer_cfg_irq_disable_Success) {
    api.hal_timer_cfg_irq_disable = [](hal_timer_t *driver, hal_timer_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_timer_cfg_irq_disable(&driver, (hal_timer_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(TimerTest, LockTimeoutAbortsOperation) {
    // Mock the lock function to simulate a timeout
    driver.base.lock = [](hal_driver_t* b, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_TIMING_TIMEOUT;
    };
    
    // Attempt the locked operation
    EXPECT_EQ(hal_timer_start(&driver), HAL_ERROR_TIMING_TIMEOUT);
}
