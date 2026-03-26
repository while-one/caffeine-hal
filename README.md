<p align="center">
  <a href="https://whileone.me">
    <img src="https://raw.githubusercontent.com/while-one/caffeine-build/main/assets/logo.png" alt="Caffeine Logo" width="50%">
  </a>
<h1 align="center">The Caffeine Framework</h1>
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
*   **Mock Library Support:** Provides a standalone `caffeine::hal-mock` STATIC library for native host testing of upper layers (SAL, Apps) without hardware dependencies. Automatically enabled via the global `CFN_BUILD_TESTS` flag.
*   **Modular Base Driver:** All peripherals inherit from a common base (`cfn_hal_base.h`), ensuring a consistent lifecycle.
*   **Thread-Safe by Design:** Optimized locking strategy using `CFN_HAL_WITH_LOCK` for clean multi-threaded RTOS environments.
*   **Pragmatic Static Analysis:** Pre-configured for `clang-format`, `clang-tidy`, and `cppcheck` (BARR-C 2018 / Allman style).

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

**Option B: Native Testing with Mocks**
Enable the global test flag to automatically expose the mock library.

```cmake
set(CFN_BUILD_TESTS ON)
FetchContent_MakeAvailable(caffeine-hal)

# Use the mock target for your unit tests
target_link_libraries(your_test_bin PRIVATE caffeine::hal caffeine::hal-mock)
```

---

## Development & Analysis

The project includes built-in targets for maintaining code quality:

*   **Format Code:** `cmake --build build --target caffeine-hal-format`
*   **Run Static Analysis:** `cmake --build build --target caffeine-hal-analyze`
*   **Run Unit Tests:** `cmake --build build --target caffeine-hal-test` (Requires `-D CFN_BUILD_TESTS=ON`)

---

## Build Environment (Docker & Local)

Use the `caffeine-build/scripts/build.sh` helper script to build your project inside a Docker container.

```bash
# Build using the native Linux stage
./caffeine-build/scripts/build.sh unit-tests-gtest

# Perform a clean build
./caffeine-build/scripts/build.sh --clean unit-tests-gtest
```

---

## The Caffeine Framework Layers

1.  **Generic Interface (`caffeine-hal`):** This repository. Definitions of the HAL and Virtual Method Tables (VMTs).
2.  **Hardware Ports ([`caffeine-hal-ports`](https://github.com/while-one/caffeine-hal-ports)):** Concrete implementations for specific vendors. Encapsulates low-level hardware definitions into target scripts (e.g., `stm32f417vgtx.cmake`).
3.  **Middleware ([`caffeine-sal`](https://github.com/while-one/caffeine-sal)):** Hardware-agnostic device drivers and connectivity abstractions.
4.  **Application ([`caffeine-app-mvp`](https://github.com/while-one/caffeine-app-mvp)):** Top-level business logic utilizing the framework.

---
## Support the Gallery

While this library is no Mondrian, it deals with a different form of **abstraction art**. Hardware abstraction is a craft of its own—one that keeps your application code portable and your debugging sessions short.

Whether **Caffeine** is fueling an elegant embedded project or just helping you wake up your hardware, you can contribute in the following ways:

* **Star & Share:** If you find this project useful, give it a ⭐ on GitHub and share it with your fellow firmware engineers. It helps others find the library and grows the Caffeine community.
* **Show & Tell:** If you are using Caffeine in a project (personal or professional), **let me know!** Hearing how it's being used is a huge motivator.
* **Propose Features:** If the library is missing a specific "brushstroke," let's design the interface together.
* **Port New Targets:** Help us expand the collection by porting the HAL to new silicon or peripheral sets.
* **Expand the HIL Lab:** Contributions go primarily toward acquiring new development boards. These serve as dedicated **Hardware-in-the-Loop** test targets, ensuring every commit remains rock-solid across our entire fleet of supported hardware.

**If my projects helped you, feel free to buy me a brew. Or if it caused you an extra debugging session, open a PR!**

<a href="https://www.buymeacoffee.com/whileone" target="_blank">
  <img src="https://img.shields.io/badge/Caffeine%20me--0077ff?style=for-the-badge&logo=buy-me-a-coffee&logoColor=white" 
       height="40" 
       style="border-radius: 5px;">
</a>&nbsp;&nbsp;&nbsp;&nbsp;
<a href="https://github.com/sponsors/while-one" target="_blank">
<img src="https://img.shields.io/badge/Sponsor--ea4aaa?style=for-the-badge&logo=github-sponsors" height="40" style="border-radius: 5px;"> </a>&nbsp;&nbsp;&nbsp;
<a href="https://github.com/while-one/caffeine-hal/compare" target="_blank">
<img src="https://img.shields.io/badge/Open%20a%20PR--orange?style=for-the-badge&logo=github&logoColor=white" height="40" style="border-radius: 5px;">
</a>

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
