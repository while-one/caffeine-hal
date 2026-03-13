/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, furnished to do so, subject to the following conditions:
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
 * @file cfn_hal_test_comp.cpp
 * @brief Unit tests for Comparator HAL wrapper.
 */

#include <gtest/gtest.h>
#include "cfn_hal_comp.h"

class CompTest : public ::testing::Test
{
  protected:
    cfn_hal_comp_t     driver{};
    cfn_hal_comp_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const void *)&api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_COMP;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(CompTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_comp_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_comp_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CompTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_comp_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_comp_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CompTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_comp_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(CompTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *user_arg, bool init) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_FAIL;
    };
    EXPECT_EQ(cfn_hal_comp_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(CompTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_comp_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(CompTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_comp_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(CompTest, ConfigSetGet)
{
    cfn_hal_comp_config_t config = { .user_config = (void *) 0x1234 };
    cfn_hal_comp_config_t read_config{};

    EXPECT_EQ(cfn_hal_comp_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_comp_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.user_config, (void *) 0x1234);
}

TEST_F(CompTest, CallbackRegister)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register =
        [](cfn_hal_driver_t *b, const void *cb, void *arg) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_comp_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(CompTest, ReadOutputSuccess)
{
    api.read_output = [](cfn_hal_comp_t *d, cfn_hal_comp_level_t *l) -> cfn_hal_error_code_t
    {
        *l = CFN_HAL_COMP_LEVEL_HIGH;
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_comp_level_t level;
    EXPECT_EQ(cfn_hal_comp_read_output(&driver, &level), CFN_HAL_ERROR_OK);
    EXPECT_EQ(level, CFN_HAL_COMP_LEVEL_HIGH);
}

TEST_F(CompTest, SetThresholdSuccess)
{
    api.set_threshold = [](cfn_hal_comp_t *d, uint32_t t) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_comp_set_threshold(&driver, 2048), CFN_HAL_ERROR_OK);
}

TEST_F(CompTest, StartStopSuccess)
{
    api.start = [](cfn_hal_comp_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.stop = [](cfn_hal_comp_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_comp_start(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_comp_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(CompTest, EventEnableDisable)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.base.event_disable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_comp_event_enable(&driver, CFN_HAL_COMP_EVENT_TRIGGER), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_comp_event_disable(&driver, CFN_HAL_COMP_EVENT_TRIGGER), CFN_HAL_ERROR_OK);
}

TEST_F(CompTest, ErrorEnableDisable)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.base.error_disable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_comp_error_enable(&driver, CFN_HAL_COMP_ERROR_GENERAL), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_comp_error_disable(&driver, CFN_HAL_COMP_ERROR_GENERAL), CFN_HAL_ERROR_OK);
}

TEST_F(CompTest, WithLockMacroWorks)
{
    api.start = [](cfn_hal_comp_t *d)
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_comp_start);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
