/**
 * @file compliance.c
 * @brief Rigorous C file to verify C11 compliance and macro safety.
 * This file uses macro-expanded calls to ensure that static analyzers
 * can catch Undefined Behavior or non-compliant syntax in macro expansions.
 */

/* Force inclusion of ALL headers to ensure total coverage */
#include "cfn_hal.h"
#include "cfn_hal_adc.h"
#include "cfn_hal_can.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_comp.h"
#include "cfn_hal_crypto.h"
#include "cfn_hal_dac.h"
#include "cfn_hal_dma.h"
#include "cfn_hal_eth.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_i2c.h"
#include "cfn_hal_i2s.h"
#include "cfn_hal_irq.h"
#include "cfn_hal_nvm.h"
#include "cfn_hal_pwm.h"
#include "cfn_hal_qspi.h"
#include "cfn_hal_rtc.h"
#include "cfn_hal_sdio.h"
#include "cfn_hal_spi.h"
#include "cfn_hal_timer.h"
#include "cfn_hal_uart.h"
#include "cfn_hal_usb.h"
#include "cfn_hal_wdt.h"

/* Dummy Implementation for Compliance Testing */

static cfn_hal_error_code_t dummy_init(cfn_hal_driver_t *base)
{
    (void)base;
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t dummy_pin_config(cfn_hal_gpio_t *port, const cfn_hal_gpio_pin_config_t *cfg)
{
    (void)port;
    (void)cfg;
    return CFN_HAL_ERROR_OK;
}

static const cfn_hal_gpio_api_t DUMMY_GPIO_API = {
    .base = {
        .init = dummy_init,
        .deinit = NULL,
        .power_state_set = NULL,
        .config_set = NULL,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = NULL,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = NULL,
    },
    .pin_config = dummy_pin_config,
    .pin_read = NULL,
    .pin_write = NULL,
    .pin_toggle = NULL,
    .port_read = NULL,
    .port_write = NULL,
};

int main(void)
{
    /* 1. Test Static Initializer Macros */
    cfn_hal_gpio_phy_t phy = { .port = (void*)0x40000000 };
    cfn_hal_gpio_t gpio = {0};
    gpio.phy = &phy;
    gpio.api = &DUMMY_GPIO_API;

    /* 2. Test Base Initialization (Macro Expansion) */
    cfn_hal_error_code_t error = cfn_hal_gpio_init(&gpio);
    if (error != CFN_HAL_ERROR_OK)
    {
        return (int)error;
    }

    /* 3. Test Complex Variadic Macro Expansion (CFN_HAL_CHECK_AND_CALL_FUNC_VARG) */
    cfn_hal_gpio_pin_config_t pin_cfg = {
        .pin_mask = CFN_HAL_GPIO_PIN_0,
        .mode = CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP
    };
    error = cfn_hal_gpio_pin_config(&gpio, &pin_cfg);

    /* 4. Test Lock Macro Expansion (CFN_HAL_WITH_LOCK) */
    cfn_hal_error_code_t lock_result;
    CFN_HAL_WITH_LOCK(&gpio, 100, lock_result, cfn_hal_gpio_pin_config, &pin_cfg);

    return (int)(lock_result + error);
}
