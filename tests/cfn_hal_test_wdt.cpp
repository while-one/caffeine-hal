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
 * @file cfn_hal_test_wdt.cpp
 * @brief Unit tests for WDT HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_wdt.h"

class WdtTest : public ::testing::Test
{
  protected:
    cfn_hal_wdt_t        driver{};
    cfn_hal_wdt_api_t    api{};
    cfn_hal_wdt_config_t dummy_config{ .timeout_ms = 1 };

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.config      = &dummy_config;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_WDT;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(WdtTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_wdt_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_wdt_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(WdtTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_wdt_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_wdt_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(WdtTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_wdt_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(WdtTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_wdt_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(WdtTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(WdtTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

TEST_F(WdtTest, ConfigValidation)
{
    cfn_hal_wdt_config_t config = {
        .timeout_ms = 1000,
        .sleep      = CFN_HAL_WDT_CONFIG_SLEEP_RUN,
        .reset      = CFN_HAL_WDT_CONFIG_RESET_CPU,
        .custom     = nullptr,
    };

    // Valid config
    EXPECT_EQ(cfn_hal_wdt_config_validate(&driver, &config), CFN_HAL_ERROR_OK);

    // NULL driver
    EXPECT_EQ(cfn_hal_wdt_config_validate(nullptr, &config), CFN_HAL_ERROR_BAD_PARAM);

    // NULL config
    EXPECT_EQ(cfn_hal_wdt_config_validate(&driver, nullptr), CFN_HAL_ERROR_BAD_PARAM);

    // Invalid enum (Sleep)
    config.sleep = CFN_HAL_WDT_CONFIG_SLEEP_MAX;
    EXPECT_EQ(cfn_hal_wdt_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.sleep = CFN_HAL_WDT_CONFIG_SLEEP_RUN;

    // Invalid enum (Reset)
    config.reset = CFN_HAL_WDT_CONFIG_RESET_MAX;
    EXPECT_EQ(cfn_hal_wdt_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
    config.reset      = CFN_HAL_WDT_CONFIG_RESET_CPU;

    // Invalid timeout
    config.timeout_ms = 0;
    EXPECT_EQ(cfn_hal_wdt_config_validate(&driver, &config), CFN_HAL_ERROR_BAD_CONFIG);
}

TEST_F(WdtTest, ConfigSetSuccess)
{
    driver.base.status  = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.config_set = [](cfn_hal_driver_t *b, const void *config) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_wdt_config_t config{ .timeout_ms = 1 };
    EXPECT_EQ(cfn_hal_wdt_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.config, &config);
}

TEST_F(WdtTest, ConfigGetSuccess)
{
    cfn_hal_wdt_config_t config{ .timeout_ms = 1 };
    cfn_hal_wdt_config_t config_out{};
    driver.config = &config;
    EXPECT_EQ(cfn_hal_wdt_config_get(&driver, &config_out), CFN_HAL_ERROR_OK);
    EXPECT_EQ(memcmp(&config, &config_out, sizeof(config)), 0);
}

TEST_F(WdtTest, CallbackRegisterSuccess)
{
    driver.base.status         = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_wdt_callback_t callback = [](cfn_hal_wdt_t *d, uint32_t ev, uint32_t err, void *arg) {};
    EXPECT_EQ(cfn_hal_wdt_callback_register(&driver, callback, nullptr), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.cb, callback);
}

TEST_F(WdtTest, PowerStateSetSuccess)
{
    driver.base.status       = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.power_state_set = [](cfn_hal_driver_t *b, cfn_hal_power_state_t state) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_OK);
}

TEST_F(WdtTest, EventEnableDisableSuccess)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_event_enable(&driver, 0x01), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_wdt_event_disable(&driver, 0x01), CFN_HAL_ERROR_OK);
}

TEST_F(WdtTest, EventGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = 0x01;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_wdt_event_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, 0x01);
}

TEST_F(WdtTest, ErrorEnableDisableSuccess)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_error_enable(&driver, 0x01), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_wdt_error_disable(&driver, 0x01), CFN_HAL_ERROR_OK);
}

TEST_F(WdtTest, ErrorGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = 0x01;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_wdt_error_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, 0x01);
}

// --- WDT Specific Extension Tests ---

TEST_F(WdtTest, FeedSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.feed           = [](cfn_hal_wdt_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_feed(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(WdtTest, StartSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.start          = [](cfn_hal_wdt_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_start(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(WdtTest, StopSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.stop           = [](cfn_hal_wdt_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_wdt_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(WdtTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.feed           = [](cfn_hal_wdt_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_wdt_feed);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
