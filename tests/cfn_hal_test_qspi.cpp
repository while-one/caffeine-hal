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
 * @file cfn_hal_test_qspi.cpp
 * @brief Quad-SPI (QSPI) HAL Unit Tests.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_qspi.h"

class QspiTest : public ::testing::Test
{
  protected:
    cfn_hal_qspi_t        driver{};
    cfn_hal_qspi_api_t    api{};
    cfn_hal_qspi_config_t dummy_config{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.config      = &dummy_config;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_QSPI;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(QspiTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_qspi_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(QspiTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_qspi_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(QspiTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_qspi_transmit(&driver, nullptr, 0), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(QspiTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *arg, cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t
    { return (phase == CFN_HAL_CONFIG_PHASE_INIT) ? CFN_HAL_ERROR_FAIL : CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_qspi_init(&driver), CFN_HAL_ERROR_FAIL);
}

// --- Lifecycle Tests ---

TEST_F(QspiTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_hal_qspi_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(QspiTest, DeinitSuccess)
{
    api.base.deinit    = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_qspi_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Operational Tests ---

TEST_F(QspiTest, CommandSequence)
{
    api.command = [](cfn_hal_qspi_t *d, const cfn_hal_qspi_cmd_t *cmd, uint32_t timeout) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    driver.base.status     = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_qspi_cmd_t cmd = {};
    EXPECT_EQ(cfn_hal_qspi_command(&driver, &cmd, 100), CFN_HAL_ERROR_OK);
}

TEST_F(QspiTest, TransmitReceive)
{
    api.transmit = [](cfn_hal_qspi_t *d, const uint8_t *data, uint32_t timeout) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };
    api.receive = [](cfn_hal_qspi_t *d, uint8_t *data, uint32_t timeout) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    uint8_t buffer[4]  = { 0 };
    EXPECT_EQ(cfn_hal_qspi_transmit(&driver, buffer, 100), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_qspi_receive(&driver, buffer, 100), CFN_HAL_ERROR_OK);
}

TEST_F(QspiTest, MemoryMappedMode)
{
    api.memory_mapped_enable = [](cfn_hal_qspi_t *d, const cfn_hal_qspi_cmd_t *cmd) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    driver.base.status     = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_qspi_cmd_t cmd = {};
    EXPECT_EQ(cfn_hal_qspi_memory_mapped_enable(&driver, &cmd), CFN_HAL_ERROR_OK);
}

TEST_F(QspiTest, AutoPollingMode)
{
    api.autopolling_enable = [](cfn_hal_qspi_t           *d,
                                const cfn_hal_qspi_cmd_t *cmd,
                                uint32_t                  match,
                                uint32_t                  mask,
                                uint32_t                  timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    driver.base.status     = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    cfn_hal_qspi_cmd_t cmd = {};
    EXPECT_EQ(cfn_hal_qspi_autopolling_enable(&driver, &cmd, 0x01, 0x01, 100), CFN_HAL_ERROR_OK);
}

// --- Event & Error Management ---

TEST_F(QspiTest, EventEnableDisable)
{
    api.base.event_enable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.base.event_disable = [](cfn_hal_driver_t *d, uint32_t mask) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_OK; };

    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    EXPECT_EQ(cfn_hal_qspi_event_enable(&driver, CFN_HAL_QSPI_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_qspi_event_disable(&driver, CFN_HAL_QSPI_EVENT_TX_COMPLETE), CFN_HAL_ERROR_OK);
}

// --- Concurrency ---

TEST_F(QspiTest, WithLockMacroWorks)
{
    api.transmit = [](cfn_hal_qspi_t *d, const uint8_t *data, uint32_t timeout) { return CFN_HAL_ERROR_OK; };
    cfn_hal_error_code_t result;
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    uint8_t dummy      = 0;
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_qspi_transmit, &dummy, 10);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
