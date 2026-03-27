# Caffeine-HAL Context & Agent Guidelines

## 1. Project Context
Caffeine-HAL is a modern, strictly C11-compliant, Header-Defined Interface Hardware Abstraction Layer (HAL) library for embedded systems (primarily ARM Cortex-M microcontrollers). 

Its core philosophy is to provide a consistent, polymorphic interface across different microcontrollers via a Virtual Method Table (VMT) architecture, while completely decoupling the generic API from the highly vendor-specific "Board Support Package" (BSP) initialization logic.

### Core Architecture
*   **Header-Defined Interface (`INTERFACE`):** The entire generic HAL is defined in header files (`include/cfn_hal_*.h`) as `static inline` wrappers around function pointers.
*   **Base Driver (`cfn_hal_base.h` / `cfn_hal_base_impl.h`):** All peripherals inherit from a common base logic. Standard operations like `init`, `deinit`, `config_set`, `callback_register`, and `power_state_set` are handled by centralized base functions.
*   **VMT Pattern:** Every peripheral (e.g., `cfn_hal_uart_t`) contains a generic `cfn_hal_driver_t` base struct and a pointer to an API struct (e.g., `cfn_hal_uart_api_t`).
*   **Mock Library Support:** Caffeine-HAL exports a standalone `caffeine::hal-mock` STATIC library. It is automatically enabled via the global `CFN_BUILD_TESTS=ON` flag.

---

## 2. Agent Coding Rules & Constraints

When contributing to, modifying, or generating code for Caffeine-HAL, you **must** adhere to the following rules:

### A. C Standard & Syntax
*   **Strict C11:** The library is strictly C11. No dynamic memory allocation is permitted.
*   **Type Safety:** Use `<stdint.h>` types exclusively (`uint32_t`, `uint8_t`, etc.).

### B. HAL Conventions
*   **Mock Library:** Standalone mock library (`caffeine::hal-mock`) containing empty VMT stubs.
    *   **Trigger:** Automatically provided if the global `CFN_BUILD_TESTS` flag is enabled.
*   **Static Initializers:** Every peripheral MUST provide a static initializer macro: `CFN_HAL_<PERIPHERAL>_INITIALIZER(api_ptr, phy_ptr, config_ptr)`.
*   **VMT Validation:** Every peripheral API struct definition MUST be followed immediately by a `CFN_HAL_VMT_CHECK(struct cfn_hal_<p>_api_s)` call.
*   **Error Handling:** Every API function must return a `cfn_hal_error_code_t`.
*   **Event/Error Splitting:** Status flags and interrupts must be split into nominal events (`cfn_hal_<p>_event_t`) and exception errors (`cfn_hal_<p>_error_t`).
*   **Naming Conventions:** `cfn_hal_<peripheral>_t`, `cfn_hal_<peripheral>_api_t`.

### C. Concurrency & Asynchronous Design
*   **Locking Policy:** Concurrency protection is the responsibility of the caller using the `CFN_HAL_WITH_LOCK` macro.
*   **Asynchronous Semantics:** Functions suffixed with `_irq` or `_dma` are fundamentally non-blocking.
*   **Polling Semantics:** Functions suffixed with `_polling` or `_read`/`_write` (if blocking) must contain a `timeout` parameter.

### D. Formatting & Style
*   **Column Limit:** 120 columns.
*   **Style:** Use the **Allman** style (opening brace on a new line).
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
*   **Code Quality:** Static analysis targets are managed via the `cfn_add_code_quality_targets()` macro in `CaffeineMacros.cmake`.
*   **Test Presets:** Unit tests are executed via the project-local `unit-tests-gtest` preset (inheriting from `base-unit-tests-gtest` in `caffeine-build`).

---

## 5. Build Environment & CI

### A. Dockerized Toolchain
All builds utilize pre-built images from the `caffeine-build` registry.

### B. Unified CI Workflow
Every repository utilizes the framework's unified quality gate script:
```bash
# Validates formatting, static analysis, and unit tests
./caffeine-build/scripts/ci.sh all
```

### C. Strict Static Analysis
A **zero-warning policy** is enforced. Any violation in `clang-tidy` or `cppcheck` (including `unmatchedSuppression`) will fail the build.

### D. Native Host Builds
Standard CMake builds are supported on the host. Ensure the global test flag is enabled if running tests:
```bash
cmake --preset unit-tests-gtest
cmake --build build/unit-tests-gtest
ctest --preset unit-tests-gtest
```
