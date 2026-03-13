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
 * @file cfn_hal_test_timer.cpp
 * @brief Unit tests for Timer HAL wrapper.
 */

#include <gtest/gtest.h>
#include "cfn_hal_timer.h"

class TimerTest : public ::testing::Test
{
protected:
    cfn_hal_timer_t     driver{};
    cfn_hal_timer_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_TIMER;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
        driver.base.vmt    = (const void *)&api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(TimerTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_timer_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_timer_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(TimerTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_timer_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_timer_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(TimerTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default
    EXPECT_EQ(cfn_hal_timer_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(TimerTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *user_arg, bool is_init) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_FAIL;
    };
    EXPECT_EQ(cfn_hal_timer_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(TimerTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_timer_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(TimerTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_timer_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(TimerTest, ConfigSetGet)
{
    cfn_hal_timer_config_t config = { .prescaler = 72, .period = 1000 };
    cfn_hal_timer_config_t read_config{};

    EXPECT_EQ(cfn_hal_timer_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_timer_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.prescaler, 72);
}

TEST_F(TimerTest, CallbackRegister)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, const void *cb, void *arg) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_timer_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(TimerTest, StartStopSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.start = [](cfn_hal_timer_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.stop = [](cfn_hal_timer_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_timer_start(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_timer_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(TimerTest, GetTicksSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.get_ticks = [](cfn_hal_timer_t *d, uint32_t ch, uint32_t *t) -> cfn_hal_error_code_t
    {
        *t = 1234;
        return CFN_HAL_ERROR_OK;
    };

    uint32_t ticks = 0;
    EXPECT_EQ(cfn_hal_timer_get_ticks(&driver, 0, &ticks), CFN_HAL_ERROR_OK);
    EXPECT_EQ(ticks, 1234);
}

TEST_F(TimerTest, GetTicksU64Success)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.get_ticks_u64 = [](cfn_hal_timer_t *d, uint32_t ch, uint64_t *t) -> cfn_hal_error_code_t
    {
        *t = 0x123456789ABCDEF0ULL;
        return CFN_HAL_ERROR_OK;
    };

    uint64_t ticks = 0;
    EXPECT_EQ(cfn_hal_timer_get_ticks_u64(&driver, 0, &ticks), CFN_HAL_ERROR_OK);
    EXPECT_EQ(ticks, 0x123456789ABCDEF0ULL);
}

TEST_F(TimerTest, SetPeriodSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.set_period = [](cfn_hal_timer_t *d, const cfn_hal_timer_period_t *p) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    cfn_hal_timer_period_t period = { .channel = 1, .ticks = 5000 };
    EXPECT_EQ(cfn_hal_timer_set_period(&driver, &period), CFN_HAL_ERROR_OK);
}

TEST_F(TimerTest, EventEnableDisable)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_timer_event_enable(&driver, CFN_HAL_TIMER_EVENT_UPDATE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_timer_event_disable(&driver, CFN_HAL_TIMER_EVENT_UPDATE), CFN_HAL_ERROR_OK);
}

TEST_F(TimerTest, ErrorEnableDisable)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_timer_error_enable(&driver, CFN_HAL_TIMER_ERROR_BREAK), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_timer_error_disable(&driver, CFN_HAL_TIMER_ERROR_BREAK), CFN_HAL_ERROR_OK);
}

TEST_F(TimerTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.start = [](cfn_hal_timer_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_timer_start);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
