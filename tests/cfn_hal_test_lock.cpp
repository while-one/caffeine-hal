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
 * @file cfn_hal_test_lock.cpp
 * @brief Unit tests for HAL locking mechanism and base driver logic.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cfn_hal_base.h"

class LockTest : public ::testing::Test
{
  protected:
    cfn_hal_driver_t   base{};
    cfn_hal_api_base_t api{};

    struct MockState
    {
        int      lock_calls   = 0;
        int      unlock_calls = 0;
        uint32_t last_timeout = 0;
        int      func_calls   = 0;
        int      func_arg_val = 0;
    } mock_state;

    void SetUp() override
    {
        memset(&base, 0, sizeof(base));
        memset(&api, 0, sizeof(api));
        memset(&mock_state, 0, sizeof(mock_state));
        base.type      = CFN_HAL_PERIPHERAL_TYPE_GPIO;
        base.status    = CFN_HAL_DRIVER_STATUS_INITIALIZED;
        base.vmt       = &api;
        base.extension = &mock_state;
    }
};

// --- Basic Lock/Unlock Tests ---

TEST_F(LockTest, LockCallsVmt)
{
    api.lock = [](cfn_hal_driver_t *b, uint32_t timeout) -> cfn_hal_error_code_t
    {
        auto *state = static_cast<MockState *>(b->extension);
        state->lock_calls++;
        state->last_timeout = timeout;
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_base_lock(&base, 123), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mock_state.lock_calls, 1);
    EXPECT_EQ(mock_state.last_timeout, 123);
}

TEST_F(LockTest, UnlockCallsVmt)
{
    api.unlock = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        auto *state = static_cast<MockState *>(b->extension);
        state->unlock_calls++;
        return CFN_HAL_ERROR_OK;
    };

    EXPECT_EQ(cfn_hal_base_unlock(&base), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mock_state.unlock_calls, 1);
}

TEST_F(LockTest, LockFailureReturnsError)
{
    api.lock = [](cfn_hal_driver_t *b, uint32_t timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_BUSY; };

    EXPECT_EQ(cfn_hal_base_lock(&base, 10), CFN_HAL_ERROR_BUSY);
}

TEST_F(LockTest, LockTimeoutReturnsError)
{
    api.lock = [](cfn_hal_driver_t *b, uint32_t timeout) -> cfn_hal_error_code_t
    { return CFN_HAL_ERROR_TIMING_TIMEOUT; };

    EXPECT_EQ(cfn_hal_base_lock(&base, 10), CFN_HAL_ERROR_TIMING_TIMEOUT);
}

TEST_F(LockTest, BareMetalFallbackWhenVmtLockNull)
{
    // api.lock and api.unlock are NULL by default from memset in SetUp
    EXPECT_EQ(cfn_hal_base_lock(&base, 10), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_hal_base_unlock(&base), CFN_HAL_ERROR_OK);
}

TEST_F(LockTest, NullBaseReturnsBadParam)
{
    EXPECT_EQ(cfn_hal_base_lock(nullptr, 10), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_hal_base_unlock(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

// --- WithLock Macro Tests ---

TEST_F(LockTest, WithLockMacroSuccess)
{
    api.lock = [](cfn_hal_driver_t *b, uint32_t timeout) -> cfn_hal_error_code_t
    {
        static_cast<MockState *>(b->extension)->lock_calls++;
        return CFN_HAL_ERROR_OK;
    };
    api.unlock = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        static_cast<MockState *>(b->extension)->unlock_calls++;
        return CFN_HAL_ERROR_OK;
    };

    auto dummy_func = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        static_cast<MockState *>(b->extension)->func_calls++;
        return CFN_HAL_ERROR_OK;
    };

    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&base, 100, result, dummy_func);

    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
    EXPECT_EQ(mock_state.lock_calls, 1);
    EXPECT_EQ(mock_state.func_calls, 1);
    EXPECT_EQ(mock_state.unlock_calls, 1);
}

TEST_F(LockTest, WithLockMacroFailureInLock)
{
    api.lock        = [](cfn_hal_driver_t *b, uint32_t timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_BUSY; };

    auto dummy_func = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t
    {
        static_cast<MockState *>(b->extension)->func_calls++;
        return CFN_HAL_ERROR_OK;
    };

    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&base, 100, result, dummy_func);

    EXPECT_EQ(result, CFN_HAL_ERROR_BUSY);
    EXPECT_EQ(mock_state.func_calls, 0); // Function should NOT be called
}

TEST_F(LockTest, WithLockMacroWithArguments)
{
    api.lock   = [](cfn_hal_driver_t *b, uint32_t timeout) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    api.unlock = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    auto dummy_func_args = [](cfn_hal_driver_t *b, int arg1, int arg2) -> cfn_hal_error_code_t
    {
        static_cast<MockState *>(b->extension)->func_arg_val = arg1 + arg2;
        return CFN_HAL_ERROR_OK;
    };

    cfn_hal_error_code_t result;
    CFN_HAL_WITH_LOCK(&base, 100, result, dummy_func_args, 10, 20);

    EXPECT_EQ(result, CFN_HAL_ERROR_OK);
    EXPECT_EQ(mock_state.func_arg_val, 30);
}

TEST_F(LockTest, WithLockMacroNullDriverReturnsBadParam)
{
    cfn_hal_error_code_t result;
    auto                 dummy_func = [](cfn_hal_driver_t *b) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    CFN_HAL_WITH_LOCK((cfn_hal_driver_t *) nullptr, 100, result, dummy_func);
    EXPECT_EQ(result, CFN_HAL_ERROR_BAD_PARAM);
}

// --- Base Lifecycle & Config Tests ---

class BaseTest : public LockTest
{
};

TEST_F(BaseTest, ConfigSetCallsVmtValidate)
{
    api.config_validate = [](const cfn_hal_driver_t *b, const void *c) -> cfn_hal_error_code_t {
        static_cast<MockState *>(b->extension)->func_calls++;
        return CFN_HAL_ERROR_OK;
    };
    api.config_set = [](cfn_hal_driver_t *b, const void *c) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };

    int dummy_config = 42;
    EXPECT_EQ(cfn_hal_base_config_set(&base, CFN_HAL_PERIPHERAL_TYPE_GPIO, &dummy_config), CFN_HAL_ERROR_OK);
    EXPECT_EQ(mock_state.func_calls, 1);
}

TEST_F(BaseTest, ConfigSetAbortsOnVmtValidateFailure)
{
    api.config_validate = [](const cfn_hal_driver_t *b, const void *c) -> cfn_hal_error_code_t {
        return CFN_HAL_ERROR_BAD_CONFIG;
    };
    api.config_set = [](cfn_hal_driver_t *b, const void *c) -> cfn_hal_error_code_t {
        static_cast<MockState *>(b->extension)->func_calls++;
        return CFN_HAL_ERROR_OK;
    };

    int dummy_config = 42;
    EXPECT_EQ(cfn_hal_base_config_set(&base, CFN_HAL_PERIPHERAL_TYPE_GPIO, &dummy_config), CFN_HAL_ERROR_BAD_CONFIG);
    EXPECT_EQ(mock_state.func_calls, 0);
}

TEST_F(BaseTest, ConfigValidateGeneric)
{
    api.config_validate = [](const cfn_hal_driver_t *b, const void *c) -> cfn_hal_error_code_t {
        return CFN_HAL_ERROR_OK;
    };
    int dummy_config = 42;
    EXPECT_EQ(cfn_hal_base_config_validate(&base, CFN_HAL_PERIPHERAL_TYPE_GPIO, &dummy_config), CFN_HAL_ERROR_OK);
}
