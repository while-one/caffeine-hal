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
 * @file cfn_hal_crypto.h
 * @brief Hardware Crypto HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_CRYPTO_H
#define CAFFEINE_HAL_HAL_CRYPTO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_base.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief Crypto nominal event flags.
 */
typedef enum
{
    CFN_HAL_CRYPTO_EVENT_NONE        = 0,
    CFN_HAL_CRYPTO_EVENT_OP_COMPLETE = CFN_HAL_BIT(0), /*!< Cryptographic operation complete */
} cfn_hal_crypto_event_t;

/**
 * @brief Crypto exception error flags.
 */
typedef enum
{
    CFN_HAL_CRYPTO_ERROR_NONE    = 0,
    CFN_HAL_CRYPTO_ERROR_AUTH    = CFN_HAL_BIT(0), /*!< Authentication/Tag mismatch error */
    CFN_HAL_CRYPTO_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_crypto_error_t;

/**
 * @brief Supported cryptographic algorithms.
 */
typedef enum
{
    CFN_HAL_CRYPTO_ALGO_AES_ECB,
    CFN_HAL_CRYPTO_ALGO_AES_CBC,
    CFN_HAL_CRYPTO_ALGO_AES_GCM,
    CFN_HAL_CRYPTO_ALGO_SHA256,
    CFN_HAL_CRYPTO_ALGO_SHA512,

    CFN_HAL_CRYPTO_ALGO_MAX
} cfn_hal_crypto_algo_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Crypto configuration structure.
 */
typedef struct
{
    cfn_hal_crypto_algo_t algo;        /*!< Default hardware algorithm */
    void                 *user_config; /*!< Vendor-specific crypto engine configuration */
} cfn_hal_crypto_config_t;

/**
 * @brief Crypto hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_crypto_phy_t;

typedef struct cfn_hal_crypto_s     cfn_hal_crypto_t;
typedef struct cfn_hal_crypto_api_s cfn_hal_crypto_api_t;

/**
 * @brief Crypto callback signature.
 * @param driver Pointer to the Crypto driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_crypto_callback_t)(cfn_hal_crypto_t *driver,
                                          uint32_t          event_mask,
                                          uint32_t          error_mask,
                                          void             *user_arg);

/**
 * @brief Crypto Virtual Method Table (VMT).
 */
struct cfn_hal_crypto_api_s
{
    cfn_hal_api_base_t base;

    /* Crypto Specific Extensions */
    cfn_hal_error_code_t (*encrypt)(cfn_hal_crypto_t *driver, const uint8_t *in, uint8_t *out, size_t size);
    cfn_hal_error_code_t (*decrypt)(cfn_hal_crypto_t *driver, const uint8_t *in, uint8_t *out, size_t size);
    cfn_hal_error_code_t (*hash_update)(cfn_hal_crypto_t *driver, const uint8_t *data, size_t size);
    cfn_hal_error_code_t (*hash_finish)(cfn_hal_crypto_t *driver, uint8_t *hash);
    cfn_hal_error_code_t (*generate_random)(cfn_hal_crypto_t *driver, uint8_t *buffer, size_t size);
    cfn_hal_error_code_t (*set_key)(cfn_hal_crypto_t *driver, const uint8_t *key, size_t key_size);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_crypto_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    crypto, cfn_hal_crypto_config_t, cfn_hal_crypto_api_t, cfn_hal_crypto_phy_t, cfn_hal_crypto_callback_t);

#define CFN_HAL_CRYPTO_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                       \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the Crypto configuration.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_config_validate(const cfn_hal_crypto_t        *driver,
                                                                   const cfn_hal_crypto_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (config->algo >= CFN_HAL_CRYPTO_ALGO_MAX)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, config);
}

/**
 * @brief Initializes the Crypto driver.
 * @param driver Pointer to the Crypto driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_init(cfn_hal_crypto_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO);
}

/**
 * @brief Deinitializes the Crypto driver.
 * @param driver Pointer to the Crypto driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_deinit(cfn_hal_crypto_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO);
}

/**
 * @brief Sets the Crypto configuration.
 * @param driver Pointer to the Crypto driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_config_set(cfn_hal_crypto_t              *driver,
                                                              const cfn_hal_crypto_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, (const void *) config);
}

/**
 * @brief Gets the current Crypto configuration.
 * @param driver Pointer to the Crypto driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_config_get(cfn_hal_crypto_t *driver, cfn_hal_crypto_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for Crypto events and errors.
 * @param driver Pointer to the Crypto driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_callback_register(cfn_hal_crypto_t               *driver,
                                                                     const cfn_hal_crypto_callback_t callback,
                                                                     void                           *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->cb          = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the Crypto power state.
 * @param driver Pointer to the Crypto driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_power_state_set(cfn_hal_crypto_t     *driver,
                                                                   cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, state);
}

/**
 * @brief Enables one or more Crypto nominal events.
 * @param driver Pointer to the Crypto driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_event_enable(cfn_hal_crypto_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, event_mask);
}

/**
 * @brief Disables one or more Crypto nominal events.
 * @param driver Pointer to the Crypto driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_event_disable(cfn_hal_crypto_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, event_mask);
}

/**
 * @brief Retrieves the current Crypto nominal event status.
 * @param driver Pointer to the Crypto driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_event_get(cfn_hal_crypto_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, event_mask);
}

/**
 * @brief Enables one or more Crypto exception errors.
 * @param driver Pointer to the Crypto driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_error_enable(cfn_hal_crypto_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, error_mask);
}

/**
 * @brief Disables one or more Crypto exception errors.
 * @param driver Pointer to the Crypto driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_error_disable(cfn_hal_crypto_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, error_mask);
}

/**
 * @brief Retrieves the current Crypto exception error status.
 * @param driver Pointer to the Crypto driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_error_get(cfn_hal_crypto_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_CRYPTO, error_mask);
}

/* CRYPTO Specific Functions ---------------------------------------- */

/**
 * @brief Encrypts a block of data.
 * @param driver Pointer to the Crypto driver instance.
 * @param in Pointer to the plaintext data.
 * @param out Pointer to the buffer where ciphertext will be stored.
 * @param size Number of bytes to encrypt.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_encrypt(cfn_hal_crypto_t *driver,
                                                           const uint8_t    *in,
                                                           uint8_t          *out,
                                                           size_t            size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, encrypt, driver, error, in, out, size);
    return error;
}

/**
 * @brief Decrypts a block of data.
 * @param driver Pointer to the Crypto driver instance.
 * @param in Pointer to the ciphertext data.
 * @param out Pointer to the buffer where plaintext will be stored.
 * @param size Number of bytes to decrypt.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_decrypt(cfn_hal_crypto_t *driver,
                                                           const uint8_t    *in,
                                                           uint8_t          *out,
                                                           size_t            size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, decrypt, driver, error, in, out, size);
    return error;
}

/**
 * @brief Feeds data into the hashing engine.
 * @param driver Pointer to the Crypto driver instance.
 * @param data Pointer to the data to be hashed.
 * @param size Number of bytes to process.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_hash_update(cfn_hal_crypto_t *driver,
                                                               const uint8_t    *data,
                                                               size_t            size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, hash_update, driver, error, data, size);
    return error;
}

/**
 * @brief Finalizes the hash calculation and retrieves the digest.
 * @param driver Pointer to the Crypto driver instance.
 * @param hash [out] Pointer to the buffer where the digest will be stored.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_hash_finish(cfn_hal_crypto_t *driver, uint8_t *hash)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, hash_finish, driver, error, hash);
    return error;
}

/**
 * @brief Generates cryptographically secure random numbers.
 * @param driver Pointer to the Crypto driver instance.
 * @param buffer [out] Pointer to the buffer to be filled with random data.
 * @param size Number of bytes to generate.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_generate_random(cfn_hal_crypto_t *driver,
                                                                   uint8_t          *buffer,
                                                                   size_t            size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, generate_random, driver, error, buffer, size);
    return error;
}

/**
 * @brief Sets the cryptographic key for symmetric algorithms.
 * @param driver Pointer to the Crypto driver instance.
 * @param key Pointer to the key data.
 * @param key_size Size of the key in bytes.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_crypto_set_key(cfn_hal_crypto_t *driver,
                                                           const uint8_t    *key,
                                                           size_t            key_size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CRYPTO, set_key, driver, error, key, key_size);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CRYPTO_H
