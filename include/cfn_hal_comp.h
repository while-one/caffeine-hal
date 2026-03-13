/**
 * @brief Comparator HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_COMP_H
#define CAFFEINE_HAL_HAL_COMP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_base.h"

typedef struct cfn_hal_comp_config_s
{
    void *user_config;
} cfn_hal_comp_config_t;

typedef struct
{
    void *port;
    void *user_arg;
} cfn_hal_comp_phy_t;

typedef enum
{
    CFN_HAL_EVENT_NONE = 0,
} cfn_hal_comp_event_t;

typedef struct cfn_hal_comp_s     cfn_hal_comp_t;
typedef struct cfn_hal_comp_api_s cfn_hal_comp_api_t;

typedef void (*cfn_hal_comp_callback_t)(cfn_hal_comp_t *driver, void *user_arg);

struct cfn_hal_comp_api_s
{
    cfn_hal_api_base_t base;
    /* Peripheral specific extensions can go here if needed */
};

CFN_HAL_CREATE_DRIVER_TYPE(
    comp, cfn_hal_comp_config_t, cfn_hal_comp_api_t, cfn_hal_comp_phy_t, cfn_hal_comp_callback_t);

/* Functions inline ------------------------------------------------- */
static inline cfn_hal_error_code_t cfn_hal_comp_init(cfn_hal_comp_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP);
}

static inline cfn_hal_error_code_t cfn_hal_comp_deinit(cfn_hal_comp_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP);
}

static inline cfn_hal_error_code_t cfn_hal_comp_config_set(cfn_hal_comp_t *driver, const cfn_hal_comp_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }

    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, (const void *) config);
}

static inline cfn_hal_error_code_t cfn_hal_comp_config_get(cfn_hal_comp_t *driver, cfn_hal_comp_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);

    return CFN_HAL_ERROR_OK;
}

static inline cfn_hal_error_code_t
cfn_hal_comp_callback_register(cfn_hal_comp_t *driver, const cfn_hal_comp_callback_t CALLBACK, void *user_arg)
{
    if (driver)
    {
        driver->cb = CALLBACK;
        driver->cb_user_arg = user_arg;
    }

    return cfn_hal_base_callback_register(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, (void *) CALLBACK, user_arg);
}

static inline cfn_hal_error_code_t cfn_hal_comp_power_state_set(cfn_hal_comp_t *driver, cfn_hal_power_state_t state)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;

    if (driver)
    {
        error = cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, state);
    }

    return error;
}

static inline cfn_hal_error_code_t cfn_hal_comp_event_enable(cfn_hal_comp_t *driver, uint32_t event_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;
    if (driver)
    {
        error = cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, event_mask);
    }

    return error;
}

static inline cfn_hal_error_code_t cfn_hal_comp_event_disable(cfn_hal_comp_t *driver, uint32_t event_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;
    if (driver)
    {
        error = cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, event_mask);
    }

    return error;
}

static inline cfn_hal_error_code_t cfn_hal_comp_event_get(cfn_hal_comp_t *driver, uint32_t *event_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;
    if (driver)
    {
        error = cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, event_mask);
    }

    return error;
}

static inline cfn_hal_error_code_t cfn_hal_comp_error_enable(cfn_hal_comp_t *driver, uint32_t error_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;
    if (driver)
    {
        error = cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, error_mask);
    }

    return error;
}

static inline cfn_hal_error_code_t cfn_hal_comp_error_disable(cfn_hal_comp_t *driver, uint32_t error_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;
    if (driver)
    {
        error = cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, error_mask);
    }

    return error;
}

static inline cfn_hal_error_code_t cfn_hal_comp_error_get(cfn_hal_comp_t *driver, uint32_t *error_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_BAD_PARAM;
    if (driver)
    {
        error = cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, error_mask);
    }

    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_COMP_H