<p align="center">
  <a href="https://whileone.me">
    <img src="https://whileone.me/images/caffeine-small.png" alt="Caffeine Logo" width="384" height="384">
  </a>
</p>

# Caffeine-HAL

<p align="center">
  <img src="https://img.shields.io/badge/C-11-blue.svg?style=flat-square&logo=c" alt="C11">
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat-square&logo=cmake&logoColor=white" alt="CMake">
  <a href="https://github.com/while-one/caffeine-hal/tags">
    <img src="https://img.shields.io/github/v/tag/while-one/caffeine-hal?style=flat-square&label=Release" alt="Latest Release">
  </a>
  <a href="https://github.com/while-one/caffeine-hal/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/while-one/caffeine-hal/ci.yml?style=flat-square&branch=main" alt="CI Status">
  </a>
  <a href="https://github.com/while-one/caffeine-hal/commits/main">
    <img src="https://img.shields.io/github/last-commit/while-one/caffeine-hal.svg?style=flat-square" alt="Last Commit">
  </a>
  <a href="https://github.com/while-one/caffeine-hal/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/while-one/caffeine-hal?style=flat-square&color=blue" alt="License: MIT">
  </a>
</p>


A modern, strictly C11-compliant, header-only Hardware Abstraction Layer (HAL) for embedded systems.

Copyright (c) 2026 Hisham Moussa Daou <https://whileone.me>

---

## Overview

**Caffeine-HAL** provides a polymorphic, Virtual Method Table (VMT) based interface for microcontrollers (primarily ARM Cortex-M). It is designed to be lightweight, thread-safe, and highly portable by decoupling the generic peripheral logic from vendor-specific board configurations.

### Key Features
*   **Zero-Copy & Header-Only:** Designed as a CMake `INTERFACE` library for easy integration.
*   **Modular Base Driver:** All peripherals inherit from a common base (`cfn_hal_base.h`), ensuring a consistent lifecycle. The base logic can be used as header-only (default) or compiled into a library to reduce code duplication in large systems.
*   **Layered FourCC Identification:** Uses FourCC codes with `CFN_HAL_PERIPHERAL_PREFIX` (reserved 'A' for the HAL layer) to uniquely identify peripheral types across system layers.
*   **Nominal/Exception Separation:** Standardized splitting of peripheral status into nominal `events` and exception `errors`.
*   **Board-Level Hooks:** Built-in `on_config` callback (using `cfn_hal_config_phase_t`) for handling clock gating, pin muxing, and DMA routing without breaking the generic API.
*   **Thread-Safe by Design:** Optimized locking strategy using `CFN_HAL_WITH_LOCK` for clean multi-threaded RTOS environments.
*   **Pragmatic Static Analysis:** Pre-configured for `clang-format`, `clang-tidy`, and `cppcheck` (BARR-C 2018 / Allman style).

---

## Architecture

Every peripheral in Caffeine-HAL follows a standard container pattern:

1.  **Generic Driver (`cfn_hal_xxx_t`):** Contains the base state (via `cfn_hal_driver_t`), configuration pointers, and VMT pointers.
2.  **Hardware API (`cfn_hal_xxx_api_t`):** A structure of function pointers (Virtual Method Table) implemented by the specific hardware port.
3.  **Physical Mapping (`cfn_hal_xxx_phy_t`):** Defines the hardware instance and pin mappings (using `cfn_hal_gpio_pin_handle_t`).

---

## Quick Start

### 1. Integration (CMake)

You can easily integrate Caffeine-HAL into your project using modern CMake.

**Option A: FetchContent (Recommended)**
Download and link the library automatically during the CMake configure step.

```cmake
include(FetchContent)
FetchContent_Declare(
    caffeine-hal
    GIT_REPOSITORY https://github.com/while-one/caffeine-hal.git
    GIT_TAG        main # Or a specific release tag
)
FetchContent_MakeAvailable(caffeine-hal)

target_link_libraries(your_app PRIVATE caffeine::hal)
```

**Option B: add_subdirectory**

Clone the repository into your project tree (e.g., `lib/caffeine-hal`) and add it.

```cmake
add_subdirectory(lib/caffeine-hal)
target_link_libraries(your_app PRIVATE caffeine::hal)
```

### 2. Basic Usage (UART Example)
```c
#include "cfn_hal_uart.h"

// 1. Define your configuration
cfn_hal_uart_config_t uart_cfg = {
    .baudrate = 115200,
    .data_len = CFN_HAL_UART_CONFIG_DATA_LEN_8,
    .stop_bits = CFN_HAL_UART_CONFIG_STOP_ONE_BIT,
    .parity = CFN_HAL_UART_CONFIG_PARITY_NONE,
};

// 2. Setup the physical mapping
cfn_hal_uart_phy_t uart_phy = {
    .instance = (void*)VENDOR_UART1_BASE, // Peripheral base address
    .tx = &(cfn_hal_gpio_pin_handle_t){.port = &vendor_gpio_port_a, .pin = CFN_HAL_GPIO_PIN_9},
    .rx = &(cfn_hal_gpio_pin_handle_t){.port = &vendor_gpio_port_a, .pin = CFN_HAL_GPIO_PIN_10},
};

// 3. Setup the driver instance using the static initializer macro (Recommended)
cfn_hal_uart_t my_uart = CFN_HAL_UART_INITIALIZER(&vendor_uart_api_impl, &uart_phy, &uart_cfg);

// 4. Initialize and use
if (cfn_hal_uart_init(&my_uart) == CFN_HAL_ERROR_OK) {
    uint8_t msg[] = "Hello World\n";
    cfn_hal_uart_tx_polling(&my_uart, msg, sizeof(msg), 1000);
}
```

---

## Development & Analysis

The project includes built-in targets for maintaining code quality:

*   **Format Code:** `make caffeine-hal-format` (Requires `clang-format`)
*   **Run Static Analysis:** `make caffeine-hal-analyze` (Runs `cppcheck` and `clang-tidy`)
*   **Verify C11 Compliance:** `make caffeine-hal-compliance` (Strict C11 check)
*   **Run Unit Tests:** `make caffeine-hal-test` (Requires `GoogleTest`)

---

## Build Environment (Docker & Local)

To ensure consistency between local development and CI, a multi-stage `Dockerfile` is provided in the **`caffeine-build`** submodule. This Docker image pre-installs all necessary toolchains and dependencies, including a pre-built GoogleTest.

### 1. Building with Docker (Recommended for CI Parity)

Use the `caffeine-build/scripts/build.sh` helper script to build your project inside a Docker container. This guarantees your build environment is identical to the CI.

```bash
# Build using the native Linux stage (default: builds all targets)
./caffeine-build/scripts/build.sh native

# To build a specific CMake target (e.g., 'caffeine-hal-format')
./caffeine-build/scripts/build.sh native caffeine-hal-format
```

### 2. Building Natively (Without Docker)

You can still build the project directly on your host machine without Docker. The CMake configuration is designed to gracefully handle missing dependencies.

*   **GTest:** If GoogleTest is not found on your system, CMake will automatically download it via `FetchContent`.
*   **Toolchains:** You will need to install your own `cmake`, `build-essential`, `clang-format`, etc. The `Dockerfile` provides a reference for the required packages.

```bash
# Standard CMake build on your host
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

---

## The Caffeine Framework Layers

Caffeine-HAL is the foundational interface within the broader **Caffeine Framework**—a strictly decoupled, layered architecture designed for modern embedded systems. This separation of concerns ensures that business logic remains completely portable across different microcontrollers and even host operating systems.

The framework is composed of the following distinct layers:

1.  **Generic Interface (`caffeine-hal`):** This repository. Header-only definitions of the Hardware Abstraction Layer and Virtual Method Tables (VMTs).
2.  **Hardware Ports ([`caffeine-hal-ports`](https://github.com/while-one/caffeine-hal-ports)):** The concrete implementations of the HAL for specific vendors (e.g., STM32, NXP, nRF, TI) and OS environments (Linux POSIX). It encapsulates vendor SDKs and provides modern CMake cross-compilation presets.
3.  **Middleware (TBD):** Device drivers (e.g., displays, sensors) and protocols (e.g., Modbus, USB stacks) that build strictly upon the generic `caffeine-hal` interface, remaining completely agnostic to the underlying hardware.
4.  **Application (TBD):** The top-level business logic, state machines, and system orchestration that utilize the middleware and HAL interfaces.

---

## Support

They say dealing with abstraction is a form of art, so I suppose that makes me an artist? Whether this caffeine fuels an elegant HAL or a deep debugging session, I appreciate you being part of the gallery.

If my projects helped you, buy me a brew or if the opposite open a PR!

<a href="https://www.buymeacoffee.com/whileone" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-blue.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
