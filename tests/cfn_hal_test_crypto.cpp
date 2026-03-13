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
 * @file cfn_hal_test_crypto.cpp
 * @brief Unit tests for Crypto HAL wrapper.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_crypto.h"

class CryptoTest : public ::testing::Test
{
  protected:
    cfn_hal_crypto_t     driver{};
    cfn_hal_crypto_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.vmt    = (const void *)&api;
        driver.base.type   = CFN_HAL_PERIPHERAL_TYPE_CRYPTO;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(CryptoTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_crypto_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_crypto_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CryptoTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_hal_crypto_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_crypto_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CryptoTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.base.power_state_set is null by default in our setup
    EXPECT_EQ(cfn_hal_crypto_power_state_set(&driver, CFN_HAL_POWER_STATE_ON), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(CryptoTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *user_arg, cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_FAIL;
    };
    EXPECT_EQ(cfn_hal_crypto_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(CryptoTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_crypto_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

TEST_F(CryptoTest, DeinitSuccess)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.deinit    = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_hal_crypto_deinit(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Configuration & Callback Tests ---

TEST_F(CryptoTest, ConfigSetGet)
{
    cfn_hal_crypto_config_t config = { .algo = CFN_HAL_CRYPTO_ALGO_AES_CBC };
    cfn_hal_crypto_config_t read_config{};

    EXPECT_EQ(cfn_hal_crypto_config_set(&driver, &config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_crypto_config_get(&driver, &read_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(read_config.algo, CFN_HAL_CRYPTO_ALGO_AES_CBC);
}

TEST_F(CryptoTest, CallbackRegister)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    api.base.callback_register =
        [](cfn_hal_driver_t *b, const void *cb, void *arg) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_crypto_callback_register(&driver, nullptr, nullptr), CFN_HAL_ERROR_OK);
}

// --- Functional Tests ---

TEST_F(CryptoTest, EncryptDecryptSuccess)
{
    api.encrypt = [](cfn_hal_crypto_t *d, const uint8_t *in, uint8_t *out, size_t s) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.decrypt = [](cfn_hal_crypto_t *d, const uint8_t *in, uint8_t *out, size_t s) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    uint8_t in[16] = {0};
    uint8_t out[16] = {0};
    EXPECT_EQ(cfn_hal_crypto_encrypt(&driver, in, out, 16), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_crypto_decrypt(&driver, in, out, 16), CFN_HAL_ERROR_OK);
}

TEST_F(CryptoTest, HashSuccess)
{
    api.hash_update = [](cfn_hal_crypto_t *d, const uint8_t *data, size_t s) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    api.hash_finish = [](cfn_hal_crypto_t *d, uint8_t *hash) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };

    uint8_t data[16] = {0};
    uint8_t hash[32] = {0};
    EXPECT_EQ(cfn_hal_crypto_hash_update(&driver, data, 16), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_crypto_hash_finish(&driver, hash), CFN_HAL_ERROR_OK);
}

TEST_F(CryptoTest, RandomSuccess)
{
    api.generate_random = [](cfn_hal_crypto_t *d, uint8_t *buf, size_t s) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    uint8_t buf[16];
    EXPECT_EQ(cfn_hal_crypto_generate_random(&driver, buf, 16), CFN_HAL_ERROR_OK);
}

TEST_F(CryptoTest, SetKeySuccess)
{
    api.set_key = [](cfn_hal_crypto_t *d, const uint8_t *key, size_t s) -> cfn_hal_error_code_t
    {
        return CFN_HAL_ERROR_OK;
    };
    uint8_t key[16] = {0};
    EXPECT_EQ(cfn_hal_crypto_set_key(&driver, key, 16), CFN_HAL_ERROR_OK);
}

TEST_F(CryptoTest, EventEnableDisable)
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

    EXPECT_EQ(cfn_hal_crypto_event_enable(&driver, CFN_HAL_CRYPTO_EVENT_OP_COMPLETE), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_crypto_event_disable(&driver, CFN_HAL_CRYPTO_EVENT_OP_COMPLETE), CFN_HAL_ERROR_OK);
}

TEST_F(CryptoTest, ErrorEnableDisable)
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

    EXPECT_EQ(cfn_hal_crypto_error_enable(&driver, CFN_HAL_CRYPTO_ERROR_AUTH), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_crypto_error_disable(&driver, CFN_HAL_CRYPTO_ERROR_AUTH), CFN_HAL_ERROR_OK);
}

TEST_F(CryptoTest, WithLockMacroWorks)
{
    api.set_key = [](cfn_hal_crypto_t *d, const uint8_t *k, size_t s)
    {
        return CFN_HAL_ERROR_OK;
    };
    cfn_hal_error_code_t result;
    uint8_t              key[16];
    CFN_HAL_WITH_LOCK(&driver, 100, result, cfn_hal_crypto_set_key, key, 16);
    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
}
