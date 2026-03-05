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
 * @file test_hal_can.cpp
 * @brief Hardware Abstraction Layer for TEST_CAN.CPP.
 */

#include <gtest/gtest.h>
#include "hal_can.h"

class CanTest : public ::testing::Test
{
  protected:
    hal_can_t     driver{};
    hal_can_api_t api{};

    void SetUp () override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_CAN;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F (CanTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ (hal_can_init (nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F (CanTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ (hal_can_deinit (&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F (CanTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_can_deinit = nullptr; // Explicitly null
    EXPECT_EQ (hal_can_deinit (&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F (CanTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [] (hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ (hal_can_init (&driver), HAL_ERROR_FAIL);
}

TEST_F (CanTest, hal_can_init_Success)
{
    api.hal_can_init = [] (hal_can_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_can_init (&driver), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_deinit_Success)
{
    api.hal_can_deinit = [] (hal_can_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_can_deinit (&driver), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_transmit_Success)
{
    api.hal_can_transmit = [] (hal_can_t *driver, const hal_can_msg_t *msg, uint32_t timeout) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    hal_can_msg_t dummy_msg{};
    EXPECT_EQ (hal_can_transmit (&driver, &dummy_msg, 0), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_receive_Success)
{
    api.hal_can_receive = [] (hal_can_t *driver, hal_can_msg_t *msg, uint32_t timeout) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    hal_can_msg_t dummy_msg{};
    EXPECT_EQ (hal_can_receive (&driver, &dummy_msg, 0), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_add_filter_Success)
{
    api.hal_can_add_filter = [] (hal_can_t *driver, uint32_t filter_id, uint32_t filter_mask) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_can_add_filter (&driver, 0, 0), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_register_cb_Success)
{
    api.hal_can_register_cb = [] (hal_can_t *driver, hal_can_callback_t cb, void *user_arg) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_can_register_cb (&driver, (hal_can_callback_t) nullptr, nullptr), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_cfg_irq_enable_Success)
{
    api.hal_can_cfg_irq_enable = [] (hal_can_t *driver, hal_can_interrupts_t irq) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_can_cfg_irq_enable (&driver, (hal_can_interrupts_t) 0), HAL_ERROR_OK);
}

TEST_F (CanTest, hal_can_cfg_irq_disable_Success)
{
    api.hal_can_cfg_irq_disable = [] (hal_can_t *driver, hal_can_interrupts_t irq) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_can_cfg_irq_disable (&driver, (hal_can_interrupts_t) 0), HAL_ERROR_OK);
}

TEST_F (CanTest, LockTimeoutAbortsOperation)
{
    // Mock the lock function to simulate a timeout
    driver.base.lock = [] (hal_driver_t *b, uint32_t timeout) -> hal_error_code_t { return HAL_ERROR_TIMING_TIMEOUT; };
    hal_can_msg_t dummy_msg{};

    // Attempt the locked operation
    EXPECT_EQ (hal_can_transmit (&driver, &dummy_msg, 0), HAL_ERROR_TIMING_TIMEOUT);
}
