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
 * @file cfn_hal_base_impl.h
 * @brief Base driver Hardware Abstraction Layer implementation logic.
 * This file should be included by cfn_hal_base.h or compiled as a standalone
 * translation unit.
 */

#ifndef CAFFEINE_HAL_HAL_BASE_IMPL_H
#define CAFFEINE_HAL_HAL_BASE_IMPL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_base.h"

/* Functions Implementation -----------------------------------------*/

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_init(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_ALREADY_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /* Board-level Initialization Hook (Phase A)
     * This must happen BEFORE the VMT init. It typically handles clock gating
     * and pin muxing via the 'on_config' callback provided by the BSP.
     */
    if (base->on_config)
    {
        error = base->on_config(base, base->on_config_arg, CFN_HAL_CONFIG_PHASE_INIT);
        if (error != CFN_HAL_ERROR_OK)
        {
            return error;
        }
    }

    /* Hardware-specific Peripheral Initialization (Phase B)
     * We cast the generic VMT to the base API type to access the init hook.
     */
    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->init)
    {
        error = api->init(base);
    }

    /* 4. Final State Update (Phase C)
     * If all stages passed, we mark the driver as ready for functional use.
     */
    if (error == CFN_HAL_ERROR_OK)
    {
        base->status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    else if (base->on_config)
    {
        /* Roll back board-level config if hardware init failed.
         * We ignore the error code from the DEINIT phase to ensure
         * the original Phase B error is returned to the caller.
         */
        (void) base->on_config(base, base->on_config_arg, CFN_HAL_CONFIG_PHASE_DEINIT);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_deinit(cfn_hal_driver_t         *base,
                                                          cfn_hal_peripheral_type_t expected_type)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status == CFN_HAL_DRIVER_STATUS_CONSTRUCTED)
    {
        return CFN_HAL_ERROR_OK;
    }

    if (base->status == CFN_HAL_DRIVER_STATUS_UNKNOWN)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /* Hardware-specific Peripheral Deinitialization (Phase A)
     * We trigger the VMT deinit first while the clocks/pins are still active.
     */
    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->deinit)
    {
        error = api->deinit(base);
    }

    /* Board-level Deinitialization Hook (Phase B)
     * Only after the peripheral logic is stopped do we release clocks and pins.
     * We use CFN_HAL_CONFIG_PHASE_DEINIT to signify teardown to the hook.
     */
    if (error == CFN_HAL_ERROR_OK)
    {
        /* Mark as constructed (not live) before releasing clocks to prevent race conditions */
        base->status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;

        if (base->on_config)
        {
            error = base->on_config(base, base->on_config_arg, CFN_HAL_CONFIG_PHASE_DEINIT);
        }
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_config_set(cfn_hal_driver_t         *base,
                                                              cfn_hal_peripheral_type_t expected_type,
                                                              const void               *config)
{
    if (!base || base->type != expected_type || !config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
        if (api && api->config_set)
        {
            error = api->config_set(base, config);
        }
        else
        {
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_callback_register(cfn_hal_driver_t         *base,
                                                                     cfn_hal_peripheral_type_t expected_type,
                                                                     cfn_hal_callback_t        callback,
                                                                     void                     *user_arg)
{
    if (!base || base->type != expected_type)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;

        if (api && api->callback_register)
        {
            error = api->callback_register(base, callback, user_arg);
        }
        else
        {
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_power_state_set(cfn_hal_driver_t         *base,
                                                              cfn_hal_peripheral_type_t expected_type,
                                                              cfn_hal_power_state_t     state)
{
    if (!base || base->type != expected_type)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->power_state == state)
    {
        return CFN_HAL_ERROR_OK;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;

        if (api && api->power_state_set)
        {
            error = api->power_state_set(base, state);
            if (error == CFN_HAL_ERROR_OK)
            {
                base->power_state = state;
            }
        }
        else
        {
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        base->power_state = state;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_power_state_t cfn_hal_power_state_get(const cfn_hal_driver_t *base)
{
    if (!base)
    {
        return CFN_HAL_POWER_STATE_UNKNOWN;
    }

    return base->power_state;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_event_enable(cfn_hal_driver_t         *base,
                                                                cfn_hal_peripheral_type_t expected_type,
                                                                uint32_t                  event_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->event_enable)
    {
        error = api->event_enable(base, event_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_event_disable(cfn_hal_driver_t         *base,
                                                                 cfn_hal_peripheral_type_t expected_type,
                                                                 uint32_t                  event_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->event_disable)
    {
        error = api->event_disable(base, event_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_event_get(cfn_hal_driver_t         *base,
                                                             cfn_hal_peripheral_type_t expected_type,
                                                             uint32_t                 *event_mask)
{
    if (!base || base->type != expected_type || !event_mask || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->event_get)
    {
        error = api->event_get(base, event_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_error_enable(cfn_hal_driver_t         *base,
                                                                cfn_hal_peripheral_type_t expected_type,
                                                                uint32_t                  error_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->error_enable)
    {
        error = api->error_enable(base, error_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_error_disable(cfn_hal_driver_t         *base,
                                                                 cfn_hal_peripheral_type_t expected_type,
                                                                 uint32_t                  error_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->error_disable)
    {
        error = api->error_disable(base, error_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_error_get(cfn_hal_driver_t         *base,
                                                             cfn_hal_peripheral_type_t expected_type,
                                                             uint32_t                 *error_mask)
{
    if (!base || base->type != expected_type || !error_mask || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->error_get)
    {
        error = api->error_get(base, error_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

#if (CFN_HAL_USE_LOCK == 1)
CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_lock(cfn_hal_driver_t *base, uint32_t timeout)
{
    if (!base || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (!api->lock)
    {
        return CFN_HAL_ERROR_OK;
    }

    return api->lock(base, timeout);
}

CFN_HAL_BASE_API cfn_hal_error_code_t cfn_hal_base_unlock(cfn_hal_driver_t *base)
{
    if (!base || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (!api->unlock)
    {
        return CFN_HAL_ERROR_OK;
    }

    return api->unlock(base);
}
#endif

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_BASE_IMPL_H
