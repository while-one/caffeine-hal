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
 * @file test_hal_irq.cpp
 * @brief Hardware Abstraction Layer for TEST_IRQ.CPP.
 */

#include <gtest/gtest.h>
#include "hal_irq.h"

class IrqTest : public ::testing::Test
{
  protected:
    hal_irq_t     driver{};
    hal_irq_api_t api{};

    void SetUp() override
    {
        driver.base.status = HAL_PERIPHERAL_STATUS_UNKNOWN;
        driver.base.type = HAL_PERIPHERAL_TYPE_IRQ;
        driver.base.on_config = nullptr;
        driver.api = &api;
    }
};

// --- Negative Tests ---

TEST_F(IrqTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(hal_irq_init(nullptr), HAL_ERROR_BAD_PARAM);
}

TEST_F(IrqTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(hal_irq_deinit(&driver), HAL_ERROR_BAD_PARAM);
}

TEST_F(IrqTest, UnimplementedApiReturnsNotSupported)
{
    api.hal_irq_deinit = nullptr; // Explicitly null
    EXPECT_EQ(hal_irq_deinit(&driver), HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(IrqTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](hal_driver_t *b, bool init) -> hal_error_code_t { return HAL_ERROR_FAIL; };
    EXPECT_EQ(hal_irq_init(&driver), HAL_ERROR_FAIL);
}

TEST_F(IrqTest, hal_irq_init_Success)
{
    api.hal_irq_init = [](hal_irq_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_init(&driver), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_deinit_Success)
{
    api.hal_irq_deinit = [](hal_irq_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_deinit(&driver), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_global_enable_Success)
{
    api.hal_irq_global_enable = [](hal_irq_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_global_enable(&driver), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_global_disable_Success)
{
    api.hal_irq_global_disable = [](hal_irq_t *driver) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_global_disable(&driver), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_enable_vector_Success)
{
    api.hal_irq_enable_vector = [](hal_irq_t *driver, uint32_t irq_id) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_enable_vector(&driver, 0), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_disable_vector_Success)
{
    api.hal_irq_disable_vector = [](hal_irq_t *driver, uint32_t irq_id) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_disable_vector(&driver, 0), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_set_priority_Success)
{
    api.hal_irq_set_priority = [](hal_irq_t *driver, uint32_t irq_id, uint32_t priority) -> hal_error_code_t
    { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_set_priority(&driver, 0, 0), HAL_ERROR_OK);
}

TEST_F(IrqTest, hal_irq_clear_pending_Success)
{
    api.hal_irq_clear_pending = [](hal_irq_t *driver, uint32_t irq_id) -> hal_error_code_t { return HAL_ERROR_OK; };
    EXPECT_EQ(hal_irq_clear_pending(&driver, 0), HAL_ERROR_OK);
}
