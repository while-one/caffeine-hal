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
 * @file cfn_hal_test_sdio.cpp
 * @brief SDIO / SDMMC HAL Unit Tests.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_sdio.h"

class SdioTest : public ::testing::Test
{
  protected:
    cfn_hal_sdio_t        driver{};
    cfn_hal_sdio_api_t    api{};
    cfn_hal_sdio_config_t dummy_config{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.config      = &dummy_config;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_SDIO;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(SdioTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_sdio_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(SdioTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_sdio_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(SdioTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_sdio_wait_card_ready(&driver, 0), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(SdioTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *arg, cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t
    { return (phase == CFN_HAL_CONFIG_PHASE_INIT) ? CFN_HAL_ERROR_FAIL : CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_sdio_init(&driver), CFN_HAL_ERROR_FAIL);
}

// --- Lifecycle Tests ---

TEST_F(SdioTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_sdio_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(SdioTest, DeinitSuccess)
{
    api.base.deinit    = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_sdio_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Operational Tests ---

TEST_F(SdioTest, SendCommand)
{
    api.send_command = [](cfn_hal_sdio_t *d, const cfn_hal_sdio_cmd_t *cmd, uint32_t *resp) -> cfn_hal_error_code_t
    {
        *resp = 0xAA55;
        return CFN_HAL_ERROR_OK;
    };

    driver.base.status          = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_sdio_cmd_t cmd      = {};
    uint32_t           response = 0;
    EXPECT_EQ(cfn_hal_sdio_send_command(&driver, &cmd, &response), CFN_HAL_ERROR_OK);
    EXPECT_EQ(response, 0xAA55);
}

TEST_F(SdioTest, ReadWriteBlocks)
{
    api.read_blocks =
        [](cfn_hal_sdio_t *d, uint8_t *buf, uint32_t addr, uint32_t count, uint32_t to) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.write_blocks =
        [](cfn_hal_sdio_t *d, const uint8_t *data, uint32_t addr, uint32_t count, uint32_t to) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    driver.base.status  = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    uint8_t buffer[512] = { 0 };
    EXPECT_EQ(cfn_hal_sdio_read_blocks(&driver, buffer, 0, 1, 100), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_sdio_write_blocks(&driver, buffer, 0, 1, 100), CFN_HAL_ERROR_OK);
}

TEST_F(SdioTest, GetCardInfo)
{
    api.get_card_info = [](cfn_hal_sdio_t *d, cfn_hal_sdio_card_info_t *info) -> cfn_hal_error_code_t
    {
        info->block_size = 512;
        return CFN_HAL_ERROR_OK;
    };

    driver.base.status            = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_sdio_card_info_t info = {};
    EXPECT_EQ(cfn_hal_sdio_get_card_info(&driver, &info), CFN_HAL_ERROR_OK);
    EXPECT_EQ(info.block_size, 512);
}

TEST_F(SdioTest, WaitCardReady)
{
    api.wait_card_ready = [](cfn_hal_sdio_t *d, uint32_t to) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status  = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_sdio_wait_card_ready(&driver, 100), CFN_HAL_ERROR_OK);
}

// --- Event Management ---

TEST_F(SdioTest, EventEnableDisable)
{
    api.base.event_enable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_sdio_event_enable(&driver, CFN_HAL_SDIO_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_sdio_event_disable(&driver, CFN_HAL_SDIO_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

// --- Concurrency ---

TEST_F(SdioTest, WithLockMacroWorks)
{
    api.wait_card_ready = [](cfn_hal_sdio_t *d, uint32_t to) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_sdio_wait_card_ready, 10);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
