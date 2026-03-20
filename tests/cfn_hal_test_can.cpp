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
 * @file cfn_hal_test_can.cpp
 * @brief Unit tests for CAN HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_can.h"

class CanTest : public ::testing::Test
{
  protected:
    cfn_hal_can_t     driver{};
    cfn_hal_can_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_CAN;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(CanTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_can_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_can_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CanTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_can_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_can_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CanTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_can_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(CanTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_can_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(CanTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_can_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(CanTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_can_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(CanTest, ConfigSetGet)
{
    cfn_hal_can_config_t config = { .baudrate = 500000 };
    cfn_hal_can_config_t read_config{};

    EXPECT_EQ(cfn_hal_can_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_can_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.baudrate, 500000);
}

TEST_F(CanTest, CallbackRegister)
{
    driver.base.status         = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_can_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(CanTest, TransmitSuccess)
{
    api.transmit = [](cfn_hal_can_t *d, const cfn_hal_can_msg_t *msg, uint32_t timeout) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_can_msg_t msg{};
    EXPECT_EQ(cfn_hal_can_transmit(&driver, &msg, 100), CFN_HAL_ERROR_OK);
}

TEST_F(CanTest, ReceiveSuccess)
{
    api.receive = [](cfn_hal_can_t *d, cfn_hal_can_msg_t *msg, uint32_t timeout) -> cfn_hal_error_code_t
    {
        msg->id = 0x123;
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_can_msg_t msg{};
    EXPECT_EQ(cfn_hal_can_receive(&driver, &msg, 100), CFN_HAL_ERROR_OK);
    EXPECT_EQ(msg.id, 0x123);
}

TEST_F(CanTest, AddFilterSuccess)
{
    api.add_filter = [](cfn_hal_can_t *d, const cfn_hal_can_filter_t *f) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_can_filter_t filter{};
    EXPECT_EQ(cfn_hal_can_add_filter(&driver, &filter), CFN_HAL_ERROR_OK);
}

TEST_F(CanTest, EventEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_can_event_enable(&driver, CFN_HAL_CAN_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_can_event_disable(&driver, CFN_HAL_CAN_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(CanTest, ErrorEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_can_error_enable(&driver, CFN_HAL_CAN_ERROR_BUS_OFF), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_can_error_disable(&driver, CFN_HAL_CAN_ERROR_BUS_OFF), CFN_HAL_ERROR_OK);
}

TEST_F(CanTest, WithLockMacroWorks)
{
    api.transmit = [](cfn_hal_can_t *d, const cfn_hal_can_msg_t *m, uint32_t t) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    cfn_hal_can_msg_t    msg;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_can_transmit, &msg, 0);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
