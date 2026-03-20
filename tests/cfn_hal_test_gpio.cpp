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
 * @file cfn_hal_test_gpio.cpp
 * @brief Unit tests for GPIO HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_gpio.h"

class GpioTest : public ::testing::Test
{
  protected:
    cfn_hal_gpio_t     driver{};
    cfn_hal_gpio_api_t api{};
    

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_GPIO;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(GpioTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_gpio_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_gpio_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(GpioTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_gpio_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_gpio_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(GpioTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_gpio_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);

    // Test new functions with null API hooks
    cfn_hal_gpio_pin_config_t cfg = { .pin_mask = CFN_HAL_GPIO_PIN_0 };
    EXPECT_EQ(cfn_hal_gpio_pin_config(&driver, &cfg), CFN_HAL_ERROR_NOT_SUPPORTED);
    EXPECT_EQ(cfn_hal_gpio_port_read(&driver, nullptr), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(GpioTest, BaseConfigReturnsNotSupported)
{
    // GPIO explicitly does not support generic base configuration.
    // Base configuration hooks are only triggered if the driver is INITIALIZED.
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;

    // We pass a non-null dummy pointer for config to satisfy basic parameter checks
    uint32_t dummy_config = 0;
    EXPECT_EQ(cfn_hal_base_config_set(&driver.base, CFN_HAL_PERIPHERAL_TYPE_GPIO, &dummy_config),
              CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(GpioTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_gpio_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(GpioTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_gpio_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(GpioTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_gpio_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(GpioTest, PinConfigSuccess)
{
    api.pin_config = [](cfn_hal_gpio_t *port, const cfn_hal_gpio_pin_config_t *cfg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    cfn_hal_gpio_pin_config_t cfg = { .pin_mask = CFN_HAL_GPIO_PIN_0, .mode = CFN_HAL_GPIO_CONFIG_MODE_INPUT };

    EXPECT_EQ(cfn_hal_gpio_pin_config(&driver, &cfg), CFN_HAL_ERROR_OK);
}

TEST_F(GpioTest, CallbackRegister)
{
    driver.base.status         = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_gpio_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(GpioTest, PinReadWriteToggleSuccess)
{
    api.pin_read = [](cfn_hal_gpio_t *d, cfn_hal_gpio_pin_t p, cfn_hal_gpio_state_t *s) -> cfn_hal_error_code_t
    {
        *s = CFN_HAL_GPIO_STATE_HIGH;
        return CFN_HAL_ERROR_OK;
    };
    api.pin_write = [](cfn_hal_gpio_t *d, cfn_hal_gpio_pin_t p, cfn_hal_gpio_state_t s) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.pin_toggle = [](cfn_hal_gpio_t *d, cfn_hal_gpio_pin_t p) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    cfn_hal_gpio_state_t state;
    EXPECT_EQ(cfn_hal_gpio_pin_read(&driver, CFN_HAL_GPIO_PIN_0, &state), CFN_HAL_ERROR_OK);
    EXPECT_EQ(state, CFN_HAL_GPIO_STATE_HIGH);
    EXPECT_EQ(cfn_hal_gpio_pin_write(&driver, CFN_HAL_GPIO_PIN_0, CFN_HAL_GPIO_STATE_LOW), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_gpio_pin_toggle(&driver, CFN_HAL_GPIO_PIN_0), CFN_HAL_ERROR_OK);
}

TEST_F(GpioTest, PortReadWriteSuccess)
{
    api.port_read = [](cfn_hal_gpio_t *d, uint32_t *val) -> cfn_hal_error_code_t
    {
        *val = 0xFFFFFFFF;
        return CFN_HAL_ERROR_OK;
    };
    api.port_write = [](cfn_hal_gpio_t *d, uint32_t mask, uint32_t val) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    uint32_t port_val = 0;
    EXPECT_EQ(cfn_hal_gpio_port_read(&driver, &port_val), CFN_HAL_ERROR_OK);
    EXPECT_EQ(port_val, 0xFFFFFFFF);
    EXPECT_EQ(cfn_hal_gpio_port_write(&driver, CFN_HAL_GPIO_PIN_0 | CFN_HAL_GPIO_PIN_1, 0x00000003), CFN_HAL_ERROR_OK);
}

TEST_F(GpioTest, EventEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_gpio_event_enable(&driver, CFN_HAL_GPIO_EVENT_RISING), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_gpio_event_disable(&driver, CFN_HAL_GPIO_EVENT_RISING), CFN_HAL_ERROR_OK);
}

TEST_F(GpioTest, ErrorEnableDisable)
{
    driver.base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    EXPECT_EQ(cfn_hal_gpio_error_enable(&driver, CFN_HAL_GPIO_ERROR_GENERAL), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_gpio_error_disable(&driver, CFN_HAL_GPIO_ERROR_GENERAL), CFN_HAL_ERROR_OK);
}

TEST_F(GpioTest, WithLockMacroWorks)
{
    api.pin_toggle = [](cfn_hal_gpio_t *d, cfn_hal_gpio_pin_t p) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_gpio_pin_toggle, CFN_HAL_GPIO_PIN_0);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
