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
 * @file cfn_hal_test_i2c.cpp
 * @brief Unit tests for I2C HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_i2c.h"

class I2cTest : public ::testing::Test
{
  protected:
    cfn_hal_i2c_t     driver{};
    cfn_hal_i2c_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_HAL_PERIPHERAL_TYPE_I2C;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(I2cTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_i2c_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_i2c_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(I2cTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_i2c_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_i2c_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(I2cTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_i2c_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(I2cTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_i2c_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(I2cTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_i2c_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(I2cTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_i2c_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

TEST_F(I2cTest, ConfigSetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.config_set = [](cfn_hal_driver_t *b, const void *config) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_config_t config{};
    EXPECT_EQ(cfn_hal_i2c_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.config, &config);
}

TEST_F(I2cTest, ConfigGetSuccess)
{
    cfn_hal_i2c_config_t config{};
    cfn_hal_i2c_config_t config_out{};
    driver.config = &config;
    EXPECT_EQ(cfn_hal_i2c_config_get(&driver, &config_out), CFN_HAL_ERROR_OK);
    EXPECT_EQ(memcmp(&config, &config_out, sizeof(config)), 0);
}

TEST_F(I2cTest, CallbackRegisterSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_callback_t callback = [](cfn_hal_i2c_t *d, uint32_t ev, uint32_t err, uint8_t *p, size_t n, void *arg) {
    };
    EXPECT_EQ(cfn_hal_i2c_callback_register(&driver, callback, nullptr), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.cb, callback);
}

TEST_F(I2cTest, PowerStateSetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.power_state_set = [](cfn_hal_driver_t *b, cfn_hal_power_state_t state) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_i2c_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, EventEnableDisableSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_i2c_event_enable(&driver, CFN_HAL_I2C_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_i2c_event_disable(&driver, CFN_HAL_I2C_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, EventGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_I2C_EVENT_TX_COMPLETE;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_i2c_event_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_I2C_EVENT_TX_COMPLETE);
}

TEST_F(I2cTest, ErrorEnableDisableSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_i2c_error_enable(&driver, CFN_HAL_I2C_ERROR_ACK), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_i2c_error_disable(&driver, CFN_HAL_I2C_ERROR_ACK), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, ErrorGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_I2C_ERROR_ACK;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_i2c_error_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_I2C_ERROR_ACK);
}

// --- I2C Specific Extension Tests ---

TEST_F(I2cTest, XfrIrqSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_irq = [](cfn_hal_i2c_t *d, const cfn_hal_i2c_transaction_t *xfr) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_transaction_t xfr{};
    EXPECT_EQ(cfn_hal_i2c_xfr_irq(&driver, &xfr), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, XfrIrqAbortSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_irq_abort = [](cfn_hal_i2c_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_i2c_xfr_irq_abort(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, XfrPollingSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_polling = [](cfn_hal_i2c_t                   *d,
                         const cfn_hal_i2c_transaction_t *xfr,
                         uint32_t                         timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_transaction_t xfr{};
    EXPECT_EQ(cfn_hal_i2c_xfr_polling(&driver, &xfr, 100), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, MemReadSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.mem_read = [](cfn_hal_i2c_t                       *d,
                      const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                      uint32_t timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_mem_transaction_t mem_xfr{};
    EXPECT_EQ(cfn_hal_i2c_mem_read(&driver, &mem_xfr, 100), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, MemWriteSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.mem_write = [](cfn_hal_i2c_t                       *d,
                       const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                       uint32_t timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_mem_transaction_t mem_xfr{};
    EXPECT_EQ(cfn_hal_i2c_mem_write(&driver, &mem_xfr, 100), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, XfrDmaSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_dma = [](cfn_hal_i2c_t *d, const cfn_hal_i2c_transaction_t *xfr) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_i2c_transaction_t xfr{};
    EXPECT_EQ(cfn_hal_i2c_xfr_dma(&driver, &xfr), CFN_HAL_ERROR_OK);
}

TEST_F(I2cTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_polling = [](cfn_hal_i2c_t *d, const cfn_hal_i2c_transaction_t *x, uint32_t t) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t      result;
    cfn_hal_i2c_transaction_t xfr{};
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_i2c_xfr_polling, &xfr, 0);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
