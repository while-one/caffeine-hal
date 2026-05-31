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
/**
 * @brief Standardizes the population of a peripheral driver structure.
 * This macro ensures that the base driver is populated and that all peripheral-specific
 * pointers (API, PHY, Config) and callbacks are assigned correctly and consistently.
 */
#define CFN_HAL_POPULATE_DRIVER(                                                                                       \
    driver_ptr, periph_type, periph_id, clock_ptr, dep_ptr, api_ptr, phy_ptr, cfg_ptr, cb_func, cb_arg)                \
    do                                                                                                                 \
    {                                                                                                                  \
        if (driver_ptr)                                                                                                \
        {                                                                                                              \
            cfn_hal_base_populate(                                                                                     \
                &(driver_ptr)->base, (periph_type), (periph_id), &(api_ptr)->base, (clock_ptr), (dep_ptr));            \
            (driver_ptr)->api         = (api_ptr);                                                                     \
            (driver_ptr)->phy         = (phy_ptr);                                                                     \
            (driver_ptr)->config      = (cfg_ptr);                                                                     \
            (driver_ptr)->cb          = (cb_func);                                                                     \
            (driver_ptr)->cb_user_arg = (cb_arg);                                                                      \
        }                                                                                                              \
    } while (0)

CFN_HAL_INLINE void
cfn_hal_base_populate (cfn_hal_driver_t                *p_base,
                       cfn_hal_peripheral_type_t        type,
                       uint32_t                         peripheral_id,
                       const struct cfn_hal_api_base_s *p_vmt,
                       struct cfn_hal_clock_s          *p_clock,
                       void                            *p_dependency)
{
    if (!p_base)
    {
        return;
    }
    p_base->type          = type;
    p_base->peripheral_id = peripheral_id;
    p_base->vmt           = p_vmt;
    p_base->clock_driver  = p_clock;
    p_base->status        = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    p_base->power_state   = CFN_HAL_POWER_STATE_UNKNOWN;
    p_base->on_config     = NULL;
    p_base->on_config_arg = NULL;
    p_base->dependency    = p_dependency;
    p_base->extension     = NULL;
    p_base->flags         = 0;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_init (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type)
{
    if (!p_base || p_base->type != expected_type || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_ALREADY_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /* Board-level Initialization Hook (Phase A)
     * This must happen BEFORE the VMT init. It typically handles clock gating
     * and pin muxing via the 'on_config' callback provided by the BSP.
     */
    if (p_base->on_config)
    {
        error = p_base->on_config(p_base, p_base->on_config_arg, CFN_HAL_CONFIG_PHASE_INIT);
        if (error != CFN_HAL_ERROR_OK)
        {
            return error;
        }
    }

    /* Hardware-specific Peripheral Initialization (Phase B)
     * We cast the generic VMT to the base API type to access the init hook.
     */
    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->init)
    {
        error = p_api->init(p_base);
    }
    else
    {
        // Requires port implmentation
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }
    /* 4. Final State Update (Phase C)
     * If all stages passed, we mark the driver as ready for functional use.
     */
    if (error == CFN_HAL_ERROR_OK)
    {
        p_base->status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    else if (p_base->on_config)
    {
        /* Roll back board-level config if hardware init failed.
         * We ignore the error code from the DEINIT phase to ensure
         * the original Phase B error is returned to the caller.
         */
        (void) p_base->on_config(p_base, p_base->on_config_arg, CFN_HAL_CONFIG_PHASE_DEINIT);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_deinit (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type)
{
    if (!p_base || p_base->type != expected_type || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status == CFN_HAL_DRIVER_STATUS_CONSTRUCTED)
    {
        return CFN_HAL_ERROR_OK;
    }

    if (p_base->status == CFN_HAL_DRIVER_STATUS_UNKNOWN)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    /* Hardware-specific Peripheral Deinitialization (Phase A)
     * We trigger the VMT deinit first while the clocks/pins are still active.
     */
    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->deinit)
    {
        error = p_api->deinit(p_base);
    }
    else
    {
        // Requires port implmentation
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }
    /* Board-level Deinitialization Hook (Phase B)
     * Only after the peripheral logic is stopped do we release clocks and pins.
     * We use CFN_HAL_CONFIG_PHASE_DEINIT to signify teardown to the hook.
     */
    if (error == CFN_HAL_ERROR_OK)
    {
        /* Mark as constructed (not live) before releasing clocks to prevent race
         * conditions */
        p_base->status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;

        if (p_base->on_config)
        {
            error = p_base->on_config(p_base, p_base->on_config_arg, CFN_HAL_CONFIG_PHASE_DEINIT);
        }
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_config_set (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, const void *p_config)
{
    if (!p_base || p_base->type != expected_type || !p_config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    if (p_base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
        if (p_api)
        {
            if (p_api->config_validate)
            {
                error = p_api->config_validate(p_base, p_config);
            }

            if (error == CFN_HAL_ERROR_OK)
            {
                if (p_api->config_set)
                {
                    error = p_api->config_set(p_base, p_config);
                }
                else
                {
                    error = CFN_HAL_ERROR_NOT_SUPPORTED;
                }
            }
        }
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_config_validate (const cfn_hal_driver_t   *p_base,
                              cfn_hal_peripheral_type_t expected_type,
                              const void               *p_config)
{
    if (!p_base || p_base->type != expected_type)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api && p_api->config_validate)
    {
        error = p_api->config_validate(p_base, p_config);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_callback_register (cfn_hal_driver_t         *p_base,
                                cfn_hal_peripheral_type_t expected_type,
                                cfn_hal_callback_t        p_callback,
                                void                     *p_user_arg)
{
    if (!p_base || p_base->type != expected_type)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    if (p_base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;

        if (p_api && p_api->callback_register)
        {
            error = p_api->callback_register(p_base, p_callback, p_user_arg);
        }
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_power_state_set (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, cfn_hal_power_state_t state)
{
    if (!p_base || p_base->type != expected_type)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->power_state == state)
    {
        return CFN_HAL_ERROR_OK;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    if (p_base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;

        if (p_api && p_api->power_state_set)
        {
            error = p_api->power_state_set(p_base, state);
            if (error == CFN_HAL_ERROR_OK)
            {
                p_base->power_state = state;
            }
        }
        else
        {
            // Requires port implmentation
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        p_base->power_state = state;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_power_state_t
cfn_hal_power_state_get (const cfn_hal_driver_t *p_base)
{
    if (!p_base)
    {
        return CFN_HAL_POWER_STATE_UNKNOWN;
    }

    return p_base->power_state;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_event_enable (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, uint32_t event_mask)
{
    if (!p_base || p_base->type != expected_type || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->event_enable)
    {
        error = p_api->event_enable(p_base, event_mask);
    }
    else
    {
        // event enable requires port implmentation
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_event_disable (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, uint32_t event_mask)
{
    if (!p_base || p_base->type != expected_type || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->event_disable)
    {
        error = p_api->event_disable(p_base, event_mask);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_event_get (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, uint32_t *p_event_mask)
{
    if (!p_base || p_base->type != expected_type || !p_event_mask || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->event_get)
    {
        error = p_api->event_get(p_base, p_event_mask);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_error_enable (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, uint32_t error_mask)
{
    if (!p_base || p_base->type != expected_type || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->error_enable)
    {
        error = p_api->error_enable(p_base, error_mask);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_error_disable (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, uint32_t error_mask)
{
    if (!p_base || p_base->type != expected_type || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->error_disable)
    {
        error = p_api->error_disable(p_base, error_mask);
    }

    return error;
}

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_error_get (cfn_hal_driver_t *p_base, cfn_hal_peripheral_type_t expected_type, uint32_t *p_error_mask)
{
    if (!p_base || p_base->type != expected_type || !p_error_mask || !p_base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (p_base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error      = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *p_api = (const cfn_hal_api_base_t *) p_base->vmt;
    if (p_api->error_get)
    {
        error = p_api->error_get(p_base, p_error_mask);
    }

    return error;
}

#if (CFN_HAL_USE_LOCK == 1)
CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_lock (cfn_hal_driver_t *base, uint32_t timeout)
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

CFN_HAL_BASE_API cfn_hal_error_code_t
cfn_hal_base_unlock (cfn_hal_driver_t *base)
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
