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
 * @file cfn_hal_test_usb.cpp
 * @brief Unit tests for USB HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_usb.h"

class UsbTest : public ::testing::Test
{
protected:
    cfn_hal_usb_t     driver{};
    cfn_hal_usb_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_USB;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
        driver.base.vmt    = (const void *)&api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(UsbTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_usb_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_usb_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(UsbTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_usb_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_usb_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(UsbTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default
    EXPECT_EQ(cfn_hal_usb_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(UsbTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *user_arg, cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_FAIL;
    };
    EXPECT_EQ(cfn_hal_usb_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(UsbTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_usb_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(UsbTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_usb_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(UsbTest, ConfigSetGet)
{
    cfn_hal_usb_config_t config = { .user_config = (void*)0x1234 };
    cfn_hal_usb_config_t read_config{};

    EXPECT_EQ(cfn_hal_usb_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.user_config, (void*)0x1234);
}

TEST_F(UsbTest, CallbackRegister)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_usb_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(UsbTest, StartStopSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.start = [](cfn_hal_usb_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.stop = [](cfn_hal_usb_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_usb_start(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, SetAddressSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.set_address = [](cfn_hal_usb_t *d, uint8_t addr) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_usb_set_address(&driver, 0x5A), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, EpOpenCloseSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.ep_open = [](cfn_hal_usb_t *d, uint8_t addr, cfn_hal_usb_ep_type_t type, uint16_t mps) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.ep_close = [](cfn_hal_usb_t *d, uint8_t addr) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_usb_ep_open(&driver, 0x01, CFN_HAL_USB_EP_TYPE_BULK, 64), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_ep_close(&driver, 0x01), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, EpTransmitReceiveSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.ep_transmit = [](cfn_hal_usb_t *d, uint8_t addr, const uint8_t *data, size_t len) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.ep_receive = [](cfn_hal_usb_t *d, uint8_t addr, uint8_t *data, size_t len) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    uint8_t buf[8];
    EXPECT_EQ(cfn_hal_usb_ep_transmit(&driver, 0x81, buf, 8), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_ep_receive(&driver, 0x01, buf, 8), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, EpStallSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.ep_stall = [](cfn_hal_usb_t *d, uint8_t addr, bool stall) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_usb_ep_stall(&driver, 0x01, true), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_ep_stall(&driver, 0x01, false), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, EventEnableDisable)
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

    EXPECT_EQ(cfn_hal_usb_event_enable(&driver, CFN_HAL_USB_EVENT_RESET), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_event_disable(&driver, CFN_HAL_USB_EVENT_RESET), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, ErrorEnableDisable)
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

    EXPECT_EQ(cfn_hal_usb_error_enable(&driver, CFN_HAL_USB_ERROR_GENERAL), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_usb_error_disable(&driver, CFN_HAL_USB_ERROR_GENERAL), CFN_HAL_ERROR_OK);
}

TEST_F(UsbTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.start = [](cfn_hal_usb_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_usb_start);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
