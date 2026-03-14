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
 * @file cfn_hal_test_spi.cpp
 * @brief Unit tests for SPI HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_spi.h"

class SpiTest : public ::testing::Test
{
  protected:
    cfn_hal_spi_t     driver{};
    cfn_hal_spi_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_HAL_PERIPHERAL_TYPE_SPI;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(SpiTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_spi_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_spi_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(SpiTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_spi_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_spi_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(SpiTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_spi_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(SpiTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t      *b,
                               void                  *user_arg,
                               cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_FAIL; };
    EXPECT_EQ(cfn_hal_spi_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(SpiTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_spi_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(SpiTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_spi_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

TEST_F(SpiTest, ConfigSetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.config_set = [](cfn_hal_driver_t *b, const void *config) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_spi_config_t config{};
    EXPECT_EQ(cfn_hal_spi_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.config, &config);
}

TEST_F(SpiTest, ConfigGetSuccess)
{
    cfn_hal_spi_config_t config{};
    cfn_hal_spi_config_t config_out{};
    driver.config = &config;
    EXPECT_EQ(cfn_hal_spi_config_get(&driver, &config_out), CFN_HAL_ERROR_OK);
    EXPECT_EQ(memcmp(&config, &config_out, sizeof(config)), 0);
}

TEST_F(SpiTest, CallbackRegisterSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register = [](cfn_hal_driver_t *b, cfn_hal_callback_t cb, void *arg) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_spi_callback_t callback =
        [](cfn_hal_spi_t *d, uint32_t ev, uint32_t err, uint8_t *data, size_t n, void *arg) {};
    EXPECT_EQ(cfn_hal_spi_callback_register(&driver, callback, nullptr), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.cb, callback);
}

TEST_F(SpiTest, PowerStateSetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.power_state_set = [](cfn_hal_driver_t *b, cfn_hal_power_state_t state) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_spi_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, EventEnableDisableSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_spi_event_enable(&driver, CFN_HAL_SPI_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_spi_event_disable(&driver, CFN_HAL_SPI_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, EventGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_SPI_EVENT_TX_COMPLETE;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_spi_event_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_SPI_EVENT_TX_COMPLETE);
}

TEST_F(SpiTest, ErrorEnableDisableSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.error_disable = [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_spi_error_enable(&driver, CFN_HAL_SPI_ERROR_OVERRUN), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_spi_error_disable(&driver, CFN_HAL_SPI_ERROR_OVERRUN), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, ErrorGetSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_get = [](cfn_hal_driver_t *b, uint32_t *mask) -> cfn_hal_error_code_t
    {
        *mask = CFN_HAL_SPI_ERROR_OVERRUN;
        return CFN_HAL_ERROR_OK;
    };
    uint32_t mask = 0;
    EXPECT_EQ(cfn_hal_spi_error_get(&driver, &mask), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mask, CFN_HAL_SPI_ERROR_OVERRUN);
}

// --- SPI Specific Extension Tests ---

TEST_F(SpiTest, XfrPollingSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_polling = [](cfn_hal_spi_t                   *d,
                         const cfn_hal_spi_transaction_t *xfr,
                         uint32_t                         timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    cfn_hal_spi_transaction_t xfr = {};
    EXPECT_EQ(cfn_hal_spi_xfr_polling(&driver, &xfr, 100), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, XfrIrqSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_irq = [](cfn_hal_spi_t *d, const cfn_hal_spi_transaction_t *xfr) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_spi_transaction_t xfr = {};
    EXPECT_EQ(cfn_hal_spi_xfr_irq(&driver, &xfr), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, XfrIrqAbortSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_irq_abort = [](cfn_hal_spi_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_spi_xfr_irq_abort(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, XfrDmaSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.xfr_dma = [](cfn_hal_spi_t *d, const cfn_hal_spi_transaction_t *xfr) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    cfn_hal_spi_transaction_t xfr = {};
    EXPECT_EQ(cfn_hal_spi_xfr_dma(&driver, &xfr), CFN_HAL_ERROR_OK);
}

TEST_F(SpiTest, WithLockMacroWorks)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_spi_init);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
