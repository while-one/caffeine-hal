# Caffeine-HAL Context & Agent Guidelines

## 1. Project Context
Caffeine-HAL is a modern, strictly C11-compliant, Header-Defined Interface Hardware Abstraction Layer (HAL) library for embedded systems (primarily ARM Cortex-M microcontrollers). 

Its core philosophy is to provide a consistent, polymorphic interface across different microcontrollers via a Virtual Method Table (VMT) architecture, while completely decoupling the generic API from the highly vendor-specific "Board Support Package" (BSP) initialization logic.

### Core Architecture
*   **Header-Defined Interface (`INTERFACE`):** The entire generic HAL is defined in header files (`include/cfn_hal_*.h`) as `static inline` wrappers around function pointers.
*   **Base Driver (`cfn_hal_base.h`):** All peripherals inherit from a common base logic. Standard operations like `init`, `deinit`, `config_set`, `callback_register`, and `power_state_set` are handled by centralized base functions.
*   **VMT Pattern:** Every peripheral (e.g., `cfn_hal_uart_t`) contains a generic `cfn_hal_driver_t` base struct and a pointer to an API struct (e.g., `cfn_hal_uart_api_t`). Hardware-specific C files implement these API structures.
*   **Board-Level Hook (`on_config`):** The `cfn_hal_driver_t` base struct contains an `on_config` callback. The generic `_init` and `_deinit` wrappers automatically call this hook before/after touching hardware registers.

---

## 2. Agent Coding Rules & Constraints

When contributing to, modifying, or generating code for Caffeine-HAL, you **must** adhere to the following rules:

### A. C Standard & Syntax
*   **Strict C11:** The library is strictly C11. Do not use C++ features, GNU extensions, or MSVC-specific attributes.
*   **No Dynamic Memory:** Never use `malloc`, `free`, or `new`.
*   **Type Safety:** Use `<stdint.h>` types exclusively (`uint32_t`, `uint8_t`, etc.).

### B. HAL Conventions
*   **Error Handling:** Every API function must return a `cfn_hal_error_code_t`.
*   **Event/Error Splitting:** Status flags and interrupts must be split into nominal events (`cfn_hal_<p>_event_t`) and exception errors (`cfn_hal_<p>_error_t`).
*   **VMT Macro Usage:** Use `CFN_HAL_CHECK_AND_CALL_FUNC` or `CFN_HAL_CHECK_AND_CALL_FUNC_VARG` inside the inline wrappers.
*   **Naming Conventions:** 
    *   Types: `cfn_hal_<peripheral>_t`, `cfn_hal_<peripheral>_api_t`
    *   Enums: `cfn_hal_<peripheral>_event_t`, `cfn_hal_<peripheral>_error_t`
    *   Physical Mapping: `cfn_hal_<peripheral>_phy_t`
    *   Porting Member: `void *instance` (Except for GPIO which uses `void *port`).
*   **GPIO Integration:** Use `cfn_hal_gpio_pin_driver_t` for any peripheral member that depends on a GPIO pin.

### C. Concurrency & Asynchronous Design
*   **Locking Policy:** Static inline functions should **not** call `CFN_HAL_LOCK` internally. Concurrency protection is the responsibility of the caller using the `CFN_HAL_WITH_LOCK` macro.
*   **Asynchronous Semantics:** Functions suffixed with `_irq` or `_dma` are fundamentally non-blocking. They **must not** contain a `timeout` parameter.
*   **Polling Semantics:** Functions suffixed with `_polling` or `_read`/`_write` (if blocking) are blocking and **must** contain a `timeout` parameter.

### D. Formatting & Style
*   **Column Limit:** 120 columns.
*   **Braces (Mandatory):** Mandatory for all control blocks.
*   **Style:** Use the **Allman** style (opening brace on a new line).
*   **Indentation:** 4 spaces. No tabs.
*   **Documentation:** All `static inline` functions must have proper Doxygen documentation.

---

## 3. Testing Requirements
*   **Test Location:** `tests/cfn_hal_test_<peripheral>.cpp`.
*   **The "Big 4" Negative Tests:** Every peripheral test suite must include:
    1.  `NullDriverReturnsBadParam`
    2.  `WrongPeripheralTypeReturnsBadParam`
    3.  `UnimplementedApiReturnsNotSupported`
    4.  `OnConfigFailureAbortsInit`

---

## 4. CMake & Build System
*   **Target Type:** Caffeine-HAL is an `INTERFACE` target.
*   **Header Registry:** New headers must be added to the `PROJECT_ALL_HEADERS` list in `CMakeLists.txt`.
