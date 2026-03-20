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
 * @file cfn_hal_test_pwm.cpp
 * @brief Pulse Width Modulation (PWM) HAL Unit Tests.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_pwm.h"

class PwmTest : public ::testing::Test
{
  protected:
    cfn_hal_pwm_t     driver{};
    cfn_hal_pwm_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_PWM;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(PwmTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_pwm_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(PwmTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_pwm_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(PwmTest, UnimplementedApiReturnsNotSupported)
{
    // api.start is nullptr by default
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_pwm_start(&driver), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(PwmTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *arg, cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t
    { return (phase == CFN_HAL_CONFIG_PHASE_INIT) ? CFN_HAL_ERROR_FAIL : CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_pwm_init(&driver), CFN_HAL_ERROR_FAIL);
}

// --- Lifecycle Tests ---

TEST_F(PwmTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_pwm_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(PwmTest, DeinitSuccess)
{
    api.base.deinit    = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_pwm_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(PwmTest, ConfigSetGet)
{
    cfn_hal_pwm_config_t config   = { 1000, 50, CFN_HAL_PWM_CONFIG_POLARITY_NORMAL, nullptr };
    cfn_hal_pwm_config_t readback = {};

    EXPECT_EQ(cfn_hal_pwm_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_pwm_config_get(&driver, &readback), CFN_HAL_ERROR_OK);
    EXPECT_EQ(readback.frequency_hz, 1000);
    EXPECT_EQ(readback.duty_cycle_percent, 50);
}

TEST_F(PwmTest, CallbackRegister)
{
    cfn_hal_pwm_callback_t callback = [](cfn_hal_pwm_t *d, uint32_t e, uint32_t err, void *arg) {};
    EXPECT_EQ(cfn_hal_pwm_callback_register(&driver, callback, (void *) 0x1234), CFN_HAL_ERROR_OK);
    EXPECT_EQ((void *) (uintptr_t) driver.cb, (void *) (uintptr_t) callback);
    EXPECT_EQ(driver.cb_user_arg, (void *) 0x1234);
}

// --- Operational Tests ---

TEST_F(PwmTest, StartStop)
{
    api.start          = [](cfn_hal_pwm_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.stop           = [](cfn_hal_pwm_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_pwm_start(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_pwm_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(PwmTest, SetFrequencyAndDuty)
{
    api.set_frequency  = [](cfn_hal_pwm_t *d, uint32_t freq) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.set_duty_cycle = [](cfn_hal_pwm_t *d, uint32_t duty) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_pwm_set_frequency(&driver, 2000), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_pwm_set_duty_cycle(&driver, 75), CFN_HAL_ERROR_OK);
}

// --- Event & Error Management ---

TEST_F(PwmTest, EventEnableDisable)
{
    api.base.event_enable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.base.event_get = [](cfn_hal_driver_t *d, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_PWM_EVENT_PERIOD_MATCH;
        return CFN_HAL_ERROR_OK;
    };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_pwm_event_enable(&driver, CFN_HAL_PWM_EVENT_PERIOD_MATCH), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_pwm_event_disable(&driver, CFN_HAL_PWM_EVENT_PERIOD_MATCH), CFN_HAL_ERROR_OK);

    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_pwm_event_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_PWM_EVENT_PERIOD_MATCH);
}

TEST_F(PwmTest, ErrorEnableDisable)
{
    api.base.error_enable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.base.error_get = [](cfn_hal_driver_t *d, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_PWM_ERROR_FAULT;
        return CFN_HAL_ERROR_OK;
    };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_pwm_error_enable(&driver, CFN_HAL_PWM_ERROR_FAULT), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_pwm_error_disable(&driver, CFN_HAL_PWM_ERROR_FAULT), CFN_HAL_ERROR_OK);

    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_pwm_error_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_PWM_ERROR_FAULT);
}

// --- Concurrency ---

TEST_F(PwmTest, WithLockMacroWorks)
{
    api.start = [](cfn_hal_pwm_t *d) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_pwm_start);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
