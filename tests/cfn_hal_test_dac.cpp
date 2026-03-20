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
 * @file cfn_hal_test_dac.cpp
 * @brief Unit tests for DAC HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_dac.h"

class DacTest : public ::testing::Test
{
  protected:
    cfn_hal_dac_t     driver{};
    cfn_hal_dac_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_DAC;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(DacTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_dac_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_dac_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(DacTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_dac_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_dac_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(DacTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_dac_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(DacTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_dac_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(DacTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_dac_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(DacTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_dac_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(DacTest, ConfigSetGet)
{
    cfn_hal_dac_config_t config = { .resolution = CFN_HAL_DAC_RESOLUTION_BIT_12 };
    cfn_hal_dac_config_t read_config{};

    EXPECT_EQ(cfn_hal_dac_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_dac_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.resolution, CFN_HAL_DAC_RESOLUTION_BIT_12);
}

TEST_F(DacTest, CallbackRegister)
{
    driver.base.status         = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_dac_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(DacTest, SetValueSuccess)
{
    api.set_value = [](cfn_hal_dac_t *d, uint32_t v) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_dac_set_value(&driver, 2048), CFN_HAL_ERROR_OK);
}

TEST_F(DacTest, StartStopSuccess)
{
    api.start = [](cfn_hal_dac_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.stop  = [](cfn_hal_dac_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_dac_start(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_dac_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(DacTest, WriteDmaSuccess)
{
    api.write_dma = [](cfn_hal_dac_t *d, const uint32_t *data, size_t nbr) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    uint32_t buffer[10] = { 0 };
    EXPECT_EQ(cfn_hal_dac_write_dma(&driver, buffer, 10), CFN_HAL_ERROR_OK);
}

TEST_F(DacTest, EventEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_dac_event_enable(&driver, CFN_HAL_DAC_EVENT_DMA_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_dac_event_disable(&driver, CFN_HAL_DAC_EVENT_DMA_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(DacTest, ErrorEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_dac_error_enable(&driver, CFN_HAL_DAC_ERROR_UNDERRUN), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_dac_error_disable(&driver, CFN_HAL_DAC_ERROR_UNDERRUN), CFN_HAL_ERROR_OK);
}

TEST_F(DacTest, WithLockMacroWorks)
{
    api.set_value = [](cfn_hal_dac_t *d, uint32_t v) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_dac_set_value, 2048);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
