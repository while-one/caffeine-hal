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
 * @file test_hal_gpio.cpp
 * @brief Hardware Abstraction Layer for TEST_GPIO.CPP.
 */

#include <gtest/gtest.h>
#include "hal_gpio.h"

class GpioTest : public ::testing::Test {
protected:
    hal_gpio_t driver{};
    hal_gpio_api_t api{};

    void SetUp() override {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_GPIO;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(GpioTest, NullDriverReturnsBadParam) {
    EXPECT_EQ(hal_gpio_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(GpioTest, WrongPeripheralTypeReturnsBadParam) {
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_gpio_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(GpioTest, UnimplementedApiReturnsNotSupported) {
    api.hal_gpio_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_gpio_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(GpioTest, OnConfigFailureAbortsInit) {
    driver.base.on_config = [](hal_driver_t* b, bool init) -> hal_error_code_t {
        return HAL_ERROR_FAIL;
    };
    EXPECT_EQ(hal_gpio_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(GpioTest, hal_gpio_register_cb_Success) {
    api.hal_gpio_register_cb = [](hal_gpio_t *driver, hal_gpio_callback_t cb, void *user_arg) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_register_cb(&driver, (hal_gpio_callback_t)nullptr, nullptr), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_set_cb_arg_Success) {
    api.hal_gpio_set_cb_arg = [](hal_gpio_t *driver, void *user_arg) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_set_cb_arg(&driver, nullptr), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_read_Success) {
    api.hal_gpio_read = [](hal_gpio_t *driver, hal_gpio_state_t *state) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_gpio_state_t dummy_state{};
    EXPECT_EQ(hal_gpio_read(&driver, &dummy_state), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_write_Success) {
    api.hal_gpio_write = [](hal_gpio_t *driver, hal_gpio_state_t state) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_write(&driver, (hal_gpio_state_t)0), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_check_error_Success) {
    api.hal_gpio_check_error = [](hal_gpio_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_check_error(&driver), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_init_Success) {
    api.hal_gpio_init = [](hal_gpio_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_init(&driver), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_deinit_Success) {
    api.hal_gpio_deinit = [](hal_gpio_t *driver) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_cfg_get_Success) {
    api.hal_gpio_cfg_get = [](hal_gpio_t *driver, hal_gpio_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_gpio_config_t dummy_config{};
    EXPECT_EQ(hal_gpio_cfg_get(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_cfg_set_Success) {
    api.hal_gpio_cfg_set = [](hal_gpio_t *driver, const hal_gpio_config_t *config) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    hal_gpio_config_t dummy_config{};
    EXPECT_EQ(hal_gpio_cfg_set(&driver, &dummy_config), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_cfg_irq_enable_Success) {
    api.hal_gpio_cfg_irq_enable = [](hal_gpio_t *driver, hal_gpio_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_cfg_irq_enable(&driver, (hal_gpio_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(GpioTest, hal_gpio_cfg_irq_disable_Success) {
    api.hal_gpio_cfg_irq_disable = [](hal_gpio_t *driver, hal_gpio_interrupts_t irq) -> hal_error_code_t {
        return HAL_ERROR_OK;
    };
    EXPECT_EQ(hal_gpio_cfg_irq_disable(&driver, (hal_gpio_interrupts_t)0), HAL_ERROR_OK);
}

TEST_F(GpioTest, LockTimeoutAbortsOperation) {
    // Mock the lock function to simulate a timeout
    driver.base.lock = [](hal_driver_t* b, uint32_t timeout) -> hal_error_code_t {
        return HAL_ERROR_TIMING_TIMEOUT;
    };
    hal_gpio_state_t dummy_state{};
    
    // Attempt the locked operation
    EXPECT_EQ(hal_gpio_read(&driver, &dummy_state), HAL_ERROR_TIMING_TIMEOUT);
}
