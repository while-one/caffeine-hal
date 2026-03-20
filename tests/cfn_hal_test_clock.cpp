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
 * @file cfn_hal_test_clock.cpp
 * @brief Unit tests for Clock HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_clock.h"

class ClockTest : public ::testing::Test
{
  protected:
    cfn_hal_clock_t     driver{};
    cfn_hal_clock_api_t api{};
    cfn_hal_clock_config_t dummy_config{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_CLOCK;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
        driver.config = &dummy_config;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(ClockTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_clock_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_clock_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(ClockTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_clock_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_clock_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(ClockTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_clock_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(ClockTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_clock_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(ClockTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_clock_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(ClockTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_clock_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(ClockTest, ConfigSetGet)
{
    cfn_hal_clock_config_t config = { .user_config = (void *) 0x1234 };
    cfn_hal_clock_config_t read_config{};

    EXPECT_EQ(cfn_hal_clock_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_clock_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.user_config, (void *) 0x1234);
}

TEST_F(ClockTest, CallbackRegister)
{
    driver.base.status         = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_clock_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(ClockTest, SuspendResumeTickSuccess)
{
    api.suspend_tick = [](cfn_hal_clock_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.resume_tick  = [](cfn_hal_clock_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_clock_suspend_tick(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_clock_resume_tick(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(ClockTest, GetSystemFreqSuccess)
{
    api.get_system_freq = [](cfn_hal_clock_t *d, uint32_t *f) -> cfn_hal_error_code_t
    {
        *f = 16000000;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t freq = 0;
    EXPECT_EQ(cfn_hal_clock_get_system_freq(&driver, &freq), CFN_HAL_ERROR_OK);
    EXPECT_EQ(freq, 16000000);
}

TEST_F(ClockTest, GetPeripheralFreqSuccess)
{
    api.get_peripheral_freq = [](cfn_hal_clock_t *d, uint32_t id, uint32_t *f) -> cfn_hal_error_code_t
    {
        *f = 8000000;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t freq = 0;
    EXPECT_EQ(cfn_hal_clock_get_peripheral_freq(&driver, 1, &freq), CFN_HAL_ERROR_OK);
    EXPECT_EQ(freq, 8000000);
}

TEST_F(ClockTest, GateEnableDisableSuccess)
{
    api.enable_gate  = [](cfn_hal_clock_t *d, uint32_t id) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.disable_gate = [](cfn_hal_clock_t *d, uint32_t id) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_clock_enable_gate(&driver, 1), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_clock_disable_gate(&driver, 1), CFN_HAL_ERROR_OK);
}

TEST_F(ClockTest, EventEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_clock_event_enable(&driver, CFN_HAL_CLOCK_EVENT_READY), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_clock_event_disable(&driver, CFN_HAL_CLOCK_EVENT_READY), CFN_HAL_ERROR_OK);
}

TEST_F(ClockTest, ErrorEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_clock_error_enable(&driver, CFN_HAL_CLOCK_ERROR_HSE_FAIL), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_clock_error_disable(&driver, CFN_HAL_CLOCK_ERROR_HSE_FAIL), CFN_HAL_ERROR_OK);
}

TEST_F(ClockTest, WithLockMacroWorks)
{
    api.suspend_tick = [](cfn_hal_clock_t *d) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_clock_suspend_tick);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
