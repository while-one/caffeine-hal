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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file hal_dma.h
 * @brief Direct Memory Access (DMA) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_DMA_H
#define CAFFEINE_HAL_HAL_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_DMA_DIR_MEM_TO_MEM,
    HAL_DMA_DIR_MEM_TO_PERIPH,
    HAL_DMA_DIR_PERIPH_TO_MEM,
    HAL_DMA_DIR_PERIPH_TO_PERIPH
} hal_dma_direction_t;

typedef enum
{
    HAL_DMA_WIDTH_8BIT,
    HAL_DMA_WIDTH_16BIT,
    HAL_DMA_WIDTH_32BIT
} hal_dma_width_t;

typedef enum
{
    HAL_DMA_INTERRUPT_TC, /* Transfer Complete */
    HAL_DMA_INTERRUPT_HT, /* Half Transfer */
    HAL_DMA_INTERRUPT_TE  /* Transfer Error */
} hal_dma_interrupts_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void    *port;
    uint32_t channel;
    void    *user_arg;
} hal_dma_phy_t;

typedef struct
{
    hal_dma_direction_t direction;
    hal_dma_width_t     src_width;
    hal_dma_width_t     dst_width;
    bool                src_inc;
    bool                dst_inc;
    void               *custom;
} hal_dma_config_t;

typedef struct hal_dma_s hal_dma_t;

typedef void (*hal_dma_callback_t) (hal_dma_t *driver, hal_dma_interrupts_t interrupt, void *user_arg);

typedef struct hal_dma_api_s hal_dma_api_t;

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_dma_api_s
{
    hal_error_code_t (*hal_dma_init) (hal_dma_t *driver);
    hal_error_code_t (*hal_dma_deinit) (hal_dma_t *driver);
    hal_error_code_t (*hal_dma_start) (hal_dma_t *driver, uint32_t src_addr, uint32_t dst_addr, uint32_t length);
    hal_error_code_t (*hal_dma_stop) (hal_dma_t *driver);
    hal_error_code_t (*hal_dma_register_cb) (hal_dma_t *driver, hal_dma_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_dma_cfg_irq_enable) (hal_dma_t *driver, hal_dma_interrupts_t irq);
    hal_error_code_t (*hal_dma_cfg_irq_disable) (hal_dma_t *driver, hal_dma_interrupts_t irq);
};

HAL_CREATE_DRIVER_TYPE (dma, hal_dma_config_t, hal_dma_api_t, hal_dma_phy_t, hal_dma_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_dma_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_init (hal_dma_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_DMA;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DMA, hal_dma_init, driver, error);
    return error;
}

/**
 * @brief hal_dma_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_deinit (hal_dma_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DMA, hal_dma_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_dma_start implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param src_addr Source memory address.
 * @param dst_addr Destination memory address.
 * @param length Number of items to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_start (hal_dma_t *driver, uint32_t src_addr, uint32_t dst_addr, uint32_t length)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DMA, hal_dma_start, driver, error, src_addr, dst_addr, length);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_dma_stop implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_stop (hal_dma_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_DMA, hal_dma_stop, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_dma_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_register_cb (hal_dma_t *driver, hal_dma_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DMA, hal_dma_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_dma_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_cfg_irq_enable (hal_dma_t *driver, hal_dma_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DMA, hal_dma_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_dma_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_dma_cfg_irq_disable (hal_dma_t *driver, hal_dma_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_DMA, hal_dma_cfg_irq_disable, driver, error, irq);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_DMA_H
