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
 * @file test_hal_usb.cpp
 * @brief Hardware Abstraction Layer for TEST_USB.CPP.
 */

#include <gtest/gtest.h>
#include "hal_usb.h"

class UsbTest : public ::testing::Test
{
  protected:
    hal_usb_t     driver{};
    hal_usb_api_t api{};

    void SetUp() override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_USB;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(UsbTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(hal_usb_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(UsbTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_usb_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(UsbTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_usb_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_usb_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(UsbTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ(hal_usb_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(UsbTest, hal_usb_init_Success)
{
    api.hal_usb_init = [](hal_usb_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_usb_init(&driver), HAL_ERROR_OK);
}

TEST_F(UsbTest, hal_usb_deinit_Success)
{
    api.hal_usb_deinit = [](hal_usb_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_usb_deinit(&driver), HAL_ERROR_OK);
}
