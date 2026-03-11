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
 * @file test_hal_nvm.cpp
 * @brief Hardware Abstraction Layer for TEST_NVM.CPP.
 */

#include <gtest/gtest.h>
#include "hal_nvm.h"

class NvmTest : public ::testing::Test
{
  protected:
    hal_nvm_t     driver{};
    hal_nvm_api_t api{};

    void SetUp() override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_NVM;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(NvmTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(hal_nvm_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(NvmTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_nvm_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(NvmTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_nvm_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_nvm_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(NvmTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ(hal_nvm_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(NvmTest, hal_nvm_init_Success)
{
    api.hal_nvm_init = [](hal_nvm_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_nvm_init(&driver), HAL_ERROR_OK);
}

TEST_F(NvmTest, hal_nvm_deinit_Success)
{
    api.hal_nvm_deinit = [](hal_nvm_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_nvm_deinit(&driver), HAL_ERROR_OK);
}
