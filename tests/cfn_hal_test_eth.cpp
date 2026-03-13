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
 * @file cfn_hal_test_eth.cpp
 * @brief Unit tests for Ethernet HAL wrapper.
 */

#include <gtest/gtest.h>
#include "cfn_hal_eth.h"

class EthTest : public ::testing::Test
{
  protected:
    cfn_hal_eth_t     driver{};
    cfn_hal_eth_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const void *)&api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_ETH;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(EthTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_eth_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_eth_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(EthTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_eth_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_eth_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(EthTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_eth_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(EthTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *user_arg, bool init) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_FAIL;
    };
    EXPECT_EQ(cfn_hal_eth_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(EthTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_eth_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(EthTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_eth_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(EthTest, ConfigSetGet)
{
    cfn_hal_eth_config_t config = { .mac_addr = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED} };
    cfn_hal_eth_config_t read_config{};

    EXPECT_EQ(cfn_hal_eth_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_eth_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.mac_addr[0], 0xDE);
}

TEST_F(EthTest, CallbackRegister)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register =
        [](cfn_hal_driver_t *b, const void *cb, void *arg) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_eth_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(EthTest, StartStopSuccess)
{
    api.start = [](cfn_hal_eth_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.stop = [](cfn_hal_eth_t *d) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_eth_start(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_eth_stop(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(EthTest, TransmitReceiveSuccess)
{
    api.transmit_frame = [](cfn_hal_eth_t *d, const uint8_t *f, size_t l) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.receive_frame = [](cfn_hal_eth_t *d, uint8_t *b, size_t m, size_t *r) -> cfn_hal_error_code_t
    {
        *r = 64;
        return CFN_HAL_ERROR_OK;
    };

    uint8_t frame[64] = {0};
    size_t  rx_len    = 0;
    EXPECT_EQ(cfn_hal_eth_transmit_frame(&driver, frame, 64), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_eth_receive_frame(&driver, frame, 64, &rx_len), CFN_HAL_ERROR_OK);
    EXPECT_EQ(rx_len, 64);
}

TEST_F(EthTest, PhyRegSuccess)
{
    api.read_phy_reg = [](cfn_hal_eth_t *d, uint16_t pa, uint16_t ra, uint16_t *v) -> cfn_hal_error_code_t
    {
        *v = 0x1234;
        return CFN_HAL_ERROR_OK;
    };
    api.write_phy_reg = [](cfn_hal_eth_t *d, uint16_t pa, uint16_t ra, uint16_t v) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    uint16_t val = 0;
    EXPECT_EQ(cfn_hal_eth_read_phy_reg(&driver, 0, 0, &val), CFN_HAL_ERROR_OK);
    EXPECT_EQ(val, 0x1234);
    EXPECT_EQ(cfn_hal_eth_write_phy_reg(&driver, 0, 0, 0x1234), CFN_HAL_ERROR_OK);
}

TEST_F(EthTest, LinkStatusSuccess)
{
    api.get_link_status = [](cfn_hal_eth_t *d, cfn_hal_eth_link_status_t *s) -> cfn_hal_error_code_t
    {
        s->is_up = true;
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_eth_link_status_t status{};
    EXPECT_EQ(cfn_hal_eth_get_link_status(&driver, &status), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(status.is_up);
}

TEST_F(EthTest, EventEnableDisable)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.event_enable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.base.event_disable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_eth_event_enable(&driver, CFN_HAL_ETH_EVENT_RX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_eth_event_disable(&driver, CFN_HAL_ETH_EVENT_RX_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(EthTest, ErrorEnableDisable)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.error_enable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.base.error_disable =
        [](cfn_hal_driver_t *b, uint32_t mask) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_eth_error_enable(&driver, CFN_HAL_ETH_ERROR_MAC), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_eth_error_disable(&driver, CFN_HAL_ETH_ERROR_MAC), CFN_HAL_ERROR_OK);
}

TEST_F(EthTest, WithLockMacroWorks)
{
    api.start = [](cfn_hal_eth_t *d)
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_eth_start);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
