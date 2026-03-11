# Caffeine-HAL Context & Agent Guidelines

## 1. Project Context
Caffeine-HAL is a modern, strictly C11-compliant, header-only Hardware Abstraction Layer (HAL) library for embedded systems (primarily ARM Cortex-M microcontrollers). 

Its core philosophy is to provide a consistent, polymorphic interface across different microcontrollers via a Virtual Method Table (VMT) architecture, while completely decoupling the generic API from the highly vendor-specific "Board Support Package" (BSP) initialization logic.

### Core Architecture
*   **Header-Only (`INTERFACE`):** The entire generic HAL is defined in header files (`include/cfn_hal_*.h`) as `static inline` wrappers around function pointers.
*   **VMT Pattern:** Every peripheral (e.g., `cfn_hal_uart_t`) contains a generic `cfn_hal_driver_t` base struct and a pointer to an API struct (e.g., `cfn_hal_uart_api_t`). Hardware-specific C files implement these API structures.
*   **Board-Level Hook (`on_config`):** The `cfn_hal_driver_t` base struct contains an `on_config` callback. The generic `_init` and `_deinit` wrappers automatically call this hook before/after touching hardware registers. This allows the user to perform critical board-level setup (like clock gating and pin muxing) cleanly.

---

## 2. Agent Coding Rules & Constraints

When contributing to, modifying, or generating code for Caffeine-HAL, you **must** adhere to the following rules:

### A. C Standard & Syntax
*   **Strict C11:** The library is strictly C11. Do not use C++ features, GNU extensions (like statement expressions or `__VA_OPT__`), or MSVC-specific attributes.
*   **No Dynamic Memory:** Never use `malloc`, `free`, or `new`. Embedded systems relying on this HAL operate with strict deterministic memory constraints.
*   **Type Safety:** Use `<stdint.h>` types exclusively (`uint32_t`, `uint8_t`, etc.). Never use bare `int`, `long`, or `char` unless required by a standard C library function.

### B. HAL Conventions
*   **Error Handling:** Every API function must return a `cfn_hal_error_code_t`. Never return data directly; pass a pointer to a variable (e.g., `uint32_t *freq_hz`) and return the status code.
*   **VMT Macro Usage:** You must use `CFN_HAL_CHECK_AND_CALL_FUNC` or `CFN_HAL_CHECK_AND_CALL_FUNC_VARG` inside the inline wrappers to invoke the VMT. This ensures `NULL` pointer safety, type-checking, and correct `CFN_HAL_ERROR_NOT_SUPPORTED` propagation.
*   **Naming Conventions:** 
    *   Types: `cfn_hal_<peripheral>_t`, `cfn_hal_<peripheral>_api_t`
    *   Enums: `cfn_hal_<peripheral>_interrupts_t`
    *   Macros: `CFN_HAL_<PERIPHERAL>_<THING>`
    *   Internal functions/macros must be prefixed to avoid namespace collisions.
*   **Driver Construction:** Port implementations MUST use the name `cfn_hal_<peripheral>_construct` (not `create`). These functions MUST manually assign `driver->base.type` and `driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;`.

### C. Concurrency & Asynchronous Design
*   **Locking:** Use `CFN_HAL_LOCK(driver, timeout)` at the start of any state-changing or hardware-accessing function, and `CFN_HAL_UNLOCK(driver)` before returning.
*   **Asynchronous Semantics:** Functions suffixed with `_irq` or `_dma` are fundamentally non-blocking. They **must not** contain a `timeout` parameter in their signature.
*   **Polling Semantics:** Functions suffixed with `_polling` are blocking and **must** contain a `timeout` parameter.

### D. Formatting & Style
*   **Column Limit:** The codebase adheres to a **120-column** width limit.
*   **Braces (Mandatory):** Every `if`, `else`, `for`, `while`, and `do` block MUST be enclosed in braces, even if it contains only a single statement (BARR-C Rule 4.2.a / MISRA Rule 15.6).
*   **Style:** Use the **Allman** style (opening brace on a new line).
*   **Types:** Never use basic C types (`int`, `short`, `long`). Always use fixed-width types from `<stdint.h>` (e.g., `uint32_t`, `int16_t`) (MISRA Rule 3.1).
*   **Indentation:** 4 spaces. Absolutely no tabs.
*   **Tooling:** If modifying C/C++ files, ensure changes pass `make format` (clang-format) and `make tidy` (clang-tidy). Warnings are treated as errors.

---

## 3. Testing Requirements
Caffeine-HAL relies on GoogleTest for comprehensive VMT wrapper validation.
*   **Test Location:** `tests/test_hal_<peripheral>.cpp`.
*   **Coverage Mandate:** Every single API wrapper function added to a header must have a corresponding test.
*   **The "Big 4" Negative Tests:** Every peripheral test suite must begin with:
    1.  `NullDriverReturnsBadParam`
    2.  `WrongPeripheralTypeReturnsBadParam`
    3.  `UnimplementedApiReturnsNotSupported`
    4.  `OnConfigFailureAbortsInit`
*   **Lock Timeout Test:** Any peripheral with asynchronous/locked functions must include a `LockTimeoutAbortsOperation` test.

---

## 4. CMake & Build System
*   **Target Type:** Caffeine-HAL is an `INTERFACE` target. Do not attempt to compile `.c` files directly into a static/shared library target for the HAL itself.
*   **Custom Targets:** The project uses `make format`, `make cppcheck`, `make tidy`, and `make analyze`. Ensure any new headers are included in these paths.
