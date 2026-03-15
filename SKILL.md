# Caffeine-HAL Context & Agent Guidelines

## 1. Project Context
Caffeine-HAL is a modern, strictly C11-compliant, Header-Defined Interface Hardware Abstraction Layer (HAL) library for embedded systems (primarily ARM Cortex-M microcontrollers). 

Its core philosophy is to provide a consistent, polymorphic interface across different microcontrollers via a Virtual Method Table (VMT) architecture, while completely decoupling the generic API from the highly vendor-specific "Board Support Package" (BSP) initialization logic.

### Core Architecture
*   **Header-Defined Interface (`INTERFACE`):** The entire generic HAL is defined in header files (`include/cfn_hal_*.h`) as `static inline` wrappers around function pointers.
*   **Base Driver (`cfn_hal_base.h` / `cfn_hal_base_impl.h`):** All peripherals inherit from a common base logic. Standard operations like `init`, `deinit`, `config_set`, `callback_register`, and `power_state_set` are handled by centralized base functions. The implementation is separated into `cfn_hal_base_impl.h` to allow for:
    *   **Header-Only (Default):** Functions are `static inline`.
    *   **Library-Linked:** Functions are `extern` by defining `CFN_HAL_USE_LIB_BASE`.
    *   **Library-Compile:** Functions are compiled into a library by defining `CFN_HAL_COMPILE_BASE`.
*   **VMT Pattern:** Every peripheral (e.g., `cfn_hal_uart_t`) contains a generic `cfn_hal_driver_t` base struct and a pointer to an API struct (e.g., `cfn_hal_uart_api_t`). The `vmt` member in the base struct is strictly typed as `const struct cfn_hal_api_base_s *` for compiler-enforced safety. Hardware-specific C files implement these API structures.
*   **VMT Safety:** To prevent memory corruption, the `cfn_hal_api_base_t` MUST be the first member of every peripheral-specific API struct. This is enforced at compile-time via the `CFN_HAL_VMT_CHECK` macro and at initialization via type-checking.
*   **Board-Level Hook (`on_config`):** The `cfn_hal_driver_t` base struct contains an `on_config` callback. The generic `_init` and `_deinit` wrappers automatically call this hook before/after touching hardware registers. 
*   **Error Preservation:** The `cfn_hal_base_init` logic ensures that the primary initialization error code from the hardware-specific driver is preserved and returned even if the board-level rollback (DEINIT phase) fails.

---

## 2. Agent Coding Rules & Constraints

When contributing to, modifying, or generating code for Caffeine-HAL, you **must** adhere to the following rules:

### A. C Standard & Syntax
*   **Strict C11:** The library is strictly C11. Do not use C++ features, GNU extensions, or MSVC-specific attributes.
*   **No Dynamic Memory:** Never use `malloc`, `free`, or `new`.
*   **Type Safety:** Use `<stdint.h>` types exclusively (`uint32_t`, `uint8_t`, etc.).
*   **Generic Callbacks:** Generic callbacks in the base layer must use `cfn_hal_callback_t` (function pointer). 
    *   **Port implementers:** You MUST cast the type-erased `cfn_hal_callback_t` back to its original signature (e.g., `cfn_hal_uart_callback_t`) before execution to avoid Undefined Behavior.

### B. HAL Conventions
*   **Static Initializers:** Every peripheral MUST provide a static initializer macro: `CFN_HAL_<PERIPHERAL>_INITIALIZER(api_ptr, phy_ptr, config_ptr)`. Use the generic `CFN_HAL_DRIVER_INITIALIZER` macro from `cfn_hal.h`.
*   **VMT Validation:** Every peripheral API struct definition MUST be followed immediately by a `CFN_HAL_VMT_CHECK(struct cfn_hal_<p>_api_s)` call to ensure structural compatibility with the base layer.
*   **Error Handling:** Every API function must return a `cfn_hal_error_code_t`.
*   **Event/Error Splitting:** Status flags and interrupts must be split into nominal events (`cfn_hal_<p>_event_t`) and exception errors (`cfn_hal_<p>_error_t`).
*   **VMT Macro Usage:** Use `CFN_HAL_CHECK_AND_CALL_FUNC` or `CFN_HAL_CHECK_AND_CALL_FUNC_VARG` inside the inline wrappers.
*   **Layered FourCC Identification:** Use `CFN_HAL_PERIPHERAL_PREFIX` (defined as 'A') for all HAL peripheral types.
*   **Naming Conventions:** 
    *   Types: `cfn_hal_<peripheral>_t`, `cfn_hal_<peripheral>_api_t`
    *   Enums: `cfn_hal_<peripheral>_event_t`, `cfn_hal_<peripheral>_error_t`
    *   Physical Mapping: `cfn_hal_<peripheral>_phy_t`
    *   Porting Member: `void *instance` (Except for GPIO which uses `void *port`).
*   **GPIO Integration:** Use `cfn_hal_gpio_pin_handle_t` for any peripheral member that depends on a GPIO pin.
*   **GPIO Bitmasking:** GPIO pins are identified by `cfn_hal_gpio_pin_t` bitmasks. 
*   **GPIO Port Controller:** The GPIO driver represents a physical PORT (e.g., GPIOA). It does not have a global configuration.
*   **GPIO Pin Configuration:** Individual pins are configured using the `cfn_hal_gpio_pin_config` function with a configuration payload.
*   **GPIO API Symmetry:** Pin-level operations (`pin_read`, `pin_write`, `pin_toggle`) must take a single pin. Port-level operations (`port_read`, `port_write`) take raw bitmasks and values.

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
*   **Header Discovery:** Headers in `include/*.h` are automatically discovered via `file(GLOB ... CONFIGURE_DEPENDS)`. No manual registry is required.

---

## 5. Build Environment & CI

The build process for Caffeine-HAL is highly optimized for consistency across local development and GitHub Actions CI using the **`caffeine-build`** submodule.

### A. Dockerized Toolchain
A multi-stage `Dockerfile` (located in the `caffeine-build` submodule) defines the entire build environment.
*   **Centralized Infrastructure:** All build images are published and hosted by the `caffeine-build` repository on the GitHub Container Registry.
*   **Sanity Checks:** CI jobs in this repository utilize these pre-built images for maximum performance.

### B. CI Workflow (`.github/workflows/ci.yml`)
The main CI pipeline runs all checks (linting, static analysis, builds, tests) inside the appropriate Docker image.
*   **Fast & Consistent Builds:** Jobs run within a pre-built Docker container from the `caffeine-build` registry.
*   **Submodule Checkout:** Every CI job MUST ensure submodules are initialized recursively to access the build infrastructure.
*   **Offline Dependencies:** CMake configuration steps include `-DFETCHCONTENT_FULLY_DISCONNECTED=ON` to leverage pre-installed dependencies and prevent network access.

### D. Local Development with Docker (`caffeine-build/scripts/build.sh`)
An optional helper script (`caffeine-build/scripts/build.sh`) allows developers to execute builds inside the Docker environment locally, ensuring perfect parity with CI.
```bash
# Example: Build natively inside Docker (default: builds all targets)
./caffeine-build/scripts/build.sh native

# Example: Run a specific CMake target (e.g., 'caffeine-hal-format')
./caffeine-build/scripts/build.sh native caffeine-hal-format

# Example: Run code coverage inside Docker
./caffeine-build/scripts/build.sh native caffeine-hal-coverage
```

### E. Native Host Builds
Developers can still build the project directly on their host machine without Docker. The CMake `Find-then-Fetch` logic for GoogleTest (`tests/CMakeLists.txt`) ensures that if GTest is not found on the host, it will be automatically downloaded, maintaining native build capability.

```bash
# Standard CMake build on your host
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```