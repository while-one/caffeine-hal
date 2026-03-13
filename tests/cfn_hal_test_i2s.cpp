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
 * @file cfn_hal_test_i2s.cpp
 * @brief Unit tests for I2S HAL wrapper.
 */

#include <gtest/gtest.h>
#include "cfn_hal_i2s.h"

class I2sTest : public ::testing::Test
{
  protected:
    cfn_hal_i2s_t     driver{};
    cfn_hal_i2s_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_I2S;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const void *)&api;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(I2sTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_i2s_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_i2s_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(I2sTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_i2s_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_i2s_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(I2sTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_i2s_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(I2sTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *user_arg, bool init) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_FAIL;
    };
    EXPECT_EQ(cfn_hal_i2s_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(I2sTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_i2s_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(I2sTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_i2s_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

TEST_F(I2sTest, ConfigSetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.config_set = [](cfn_hal_driver_t *b, const void *config) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_i2s_config_t config{};
    EXPECT_EQ(cfn_hal_i2s_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.config, &config);
}

TEST_F(I2sTest, ConfigGetSuccess)
{
    cfn_hal_i2s_config_t config{};
    cfn_hal_i2s_config_t config_out{};
    driver.config = &config;
    EXPECT_EQ(cfn_hal_i2s_config_get(&driver, &config_out), CFN_HAL_ERROR_OK);
    EXPECT_EQ(memcmp(&config, &config_out, sizeof(config)), 0);
}

TEST_F(I2sTest, CallbackRegisterSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, const void *cb, void *user_arg) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_i2s_callback_t callback = [](cfn_hal_i2s_t *d, uint32_t ev, uint32_t err, void *arg) {};
    EXPECT_EQ(cfn_hal_i2s_callback_register(&driver, callback, nullptr), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.cb, callback);
}

TEST_F(I2sTest, PowerStateSetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.power_state_set = [](cfn_hal_driver_t *b, cfn_hal_power_state_t state) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_i2s_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, EventEnableDisableSuccess)
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
    EXPECT_EQ(cfn_hal_i2s_event_enable(&driver, CFN_HAL_I2S_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_i2s_event_disable(&driver, CFN_HAL_I2S_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, EventGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_I2S_EVENT_TX_COMPLETE;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_i2s_event_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_I2S_EVENT_TX_COMPLETE);
}

TEST_F(I2sTest, ErrorEnableDisableSuccess)
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
    EXPECT_EQ(cfn_hal_i2s_error_enable(&driver, CFN_HAL_I2S_ERROR_OVERRUN), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_i2s_error_disable(&driver, CFN_HAL_I2S_ERROR_OVERRUN), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, ErrorGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_I2S_ERROR_OVERRUN;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_i2s_error_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_I2S_ERROR_OVERRUN);
}

// --- I2S Specific Extension Tests ---

TEST_F(I2sTest, TransmitDmaSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.transmit_dma = [](cfn_hal_i2s_t *d, const uint16_t *data, size_t size) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    uint16_t data[10];
    EXPECT_EQ(cfn_hal_i2s_transmit_dma(&driver, data, 10), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, ReceiveDmaSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.receive_dma = [](cfn_hal_i2s_t *d, uint16_t *data, size_t size) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    uint16_t data[10];
    EXPECT_EQ(cfn_hal_i2s_receive_dma(&driver, data, 10), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, PauseSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.pause = [](cfn_hal_i2s_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_i2s_pause(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, ResumeSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.resume = [](cfn_hal_i2s_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_i2s_resume(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, StopSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.stop = [](cfn_hal_i2s_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_i2s_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(I2sTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_i2s_init);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
