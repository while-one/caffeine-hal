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
 * @file test_hal_pwm.cpp
 * @brief Hardware Abstraction Layer for TEST_PWM.CPP.
 */

#include <gtest/gtest.h>
#include "hal_pwm.h"

class PwmTest : public ::testing::Test
{
  protected:
    hal_pwm_t     driver{};
    hal_pwm_api_t api{};

    void SetUp () override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_PWM;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F (PwmTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ (hal_pwm_init (nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F (PwmTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ (hal_pwm_deinit (&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F (PwmTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_pwm_deinit = nullptr; // Explicitly null
    EXPECT_EQ (hal_pwm_deinit (&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F (PwmTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [] (hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ (hal_pwm_init (&driver), HAL_ERROR_FAIL);
}

TEST_F (PwmTest, hal_pwm_init_Success)
{
    api.hal_pwm_init = [] (hal_pwm_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_init (&driver), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_deinit_Success)
{
    api.hal_pwm_deinit = [] (hal_pwm_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_deinit (&driver), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_start_Success)
{
    api.hal_pwm_start = [] (hal_pwm_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_start (&driver), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_stop_Success)
{
    api.hal_pwm_stop = [] (hal_pwm_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_stop (&driver), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_set_frequency_Success)
{
    api.hal_pwm_set_frequency = [] (hal_pwm_t *driver, uint32_t frequency_hz) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_set_frequency (&driver, 0), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_set_duty_cycle_Success)
{
    api.hal_pwm_set_duty_cycle = [] (hal_pwm_t *driver, uint32_t duty_percent) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_set_duty_cycle (&driver, 0), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_register_cb_Success)
{
    api.hal_pwm_register_cb = [] (hal_pwm_t *driver, hal_pwm_callback_t cb, void *user_arg) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_register_cb (&driver, (hal_pwm_callback_t) nullptr, nullptr), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_cfg_irq_enable_Success)
{
    api.hal_pwm_cfg_irq_enable = [] (hal_pwm_t *driver, hal_pwm_interrupts_t irq) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_cfg_irq_enable (&driver, (hal_pwm_interrupts_t) 0), HAL_ERROR_OK);
}

TEST_F (PwmTest, hal_pwm_cfg_irq_disable_Success)
{
    api.hal_pwm_cfg_irq_disable = [] (hal_pwm_t *driver, hal_pwm_interrupts_t irq) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ (hal_pwm_cfg_irq_disable (&driver, (hal_pwm_interrupts_t) 0), HAL_ERROR_OK);
}

TEST_F (PwmTest, LockTimeoutAbortsOperation)
{
    // Mock the lock function to simulate a timeout
    driver.base.lock = [] (hal_driver_t *b, uint32_t timeout) -> hal_error_code_t { return HAL_ERROR_TIMING_TIMEOUT; };

    // Attempt the locked operation
    EXPECT_EQ (hal_pwm_start (&driver), HAL_ERROR_TIMING_TIMEOUT);
}
