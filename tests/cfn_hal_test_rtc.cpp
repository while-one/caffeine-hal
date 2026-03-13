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
 * @file cfn_hal_test_rtc.cpp
 * @brief Real-Time Clock (RTC) HAL Unit Tests.
 */

#include <gtest/gtest.h>
#include "cfn_hal_rtc.h"

class RtcTest : public ::testing::Test
{
  protected:
    cfn_hal_rtc_t     driver{};
    cfn_hal_rtc_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const void *)&api;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_RTC;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(RtcTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_rtc_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(RtcTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_rtc_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(RtcTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_rtc_set_time(&driver, nullptr), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(RtcTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *arg, bool is_init) -> cfn_hal_error_code_t
    {
        return is_init ? CFN_HAL_ERROR_FAIL : CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_rtc_init(&driver), CFN_HAL_ERROR_FAIL);
}

// --- Lifecycle Tests ---

TEST_F(RtcTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_rtc_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(RtcTest, DeinitSuccess)
{
    api.base.deinit = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_rtc_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Operational Tests ---

TEST_F(RtcTest, SetGetTime)
{
    api.set_time = [](cfn_hal_rtc_t *d, cfn_hal_rtc_time_t *time) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.get_time = [](cfn_hal_rtc_t *d, cfn_hal_rtc_time_t *time) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_rtc_time_t time = {};
    EXPECT_EQ(cfn_hal_rtc_set_time(&driver, &time), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_rtc_get_time(&driver, &time), CFN_HAL_ERROR_OK);
}

TEST_F(RtcTest, AlarmManagement)
{
    api.set_alarm = [](cfn_hal_rtc_t *d, uint32_t id, cfn_hal_rtc_time_t *time) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.get_alarm = [](cfn_hal_rtc_t *d, uint32_t id, bool *elapsed, cfn_hal_rtc_time_t *time) -> cfn_hal_error_code_t
    {
        *elapsed = true;
        return CFN_HAL_ERROR_OK;
    };
    api.stop_alarm = [](cfn_hal_rtc_t *d, uint32_t id) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_rtc_time_t time = {};
    bool               elapsed = false;

    EXPECT_EQ(cfn_hal_rtc_set_alarm(&driver, 1, &time), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_rtc_get_alarm(&driver, 1, &elapsed, &time), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(elapsed);
    EXPECT_EQ(cfn_hal_rtc_stop_alarm(&driver, 1), CFN_HAL_ERROR_OK);
}

// --- Event Management ---

TEST_F(RtcTest, EventEnableDisable)
{
    api.base.event_enable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_rtc_event_enable(&driver, CFN_HAL_RTC_EVENT_ALARM), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_rtc_event_disable(&driver, CFN_HAL_RTC_EVENT_ALARM), CFN_HAL_ERROR_OK);
}

// --- Concurrency ---

TEST_F(RtcTest, WithLockMacroWorks)
{
    api.set_time = [](cfn_hal_rtc_t *d, cfn_hal_rtc_time_t *t) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_rtc_time_t dummy_time = {};
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_rtc_set_time, &dummy_time);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
