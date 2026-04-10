#################################### Config ####################################

# Minimum required version updated to 3.23 to support FILE_SET
cmake_minimum_required(VERSION 3.23)

# Information about the project
project(
        caffeine-hal
        VERSION 0.6.1
        LANGUAGES C
        DESCRIPTION "A Header-Defined Interface c library, it provides the hal layer for the Caffeine framework"
        HOMEPAGE_URL "https://github.com/while-one/caffeine-hal"
)

# This provides some important cross-platform paths to be used later during install
include(GNUInstallDirs)

# 3. Code Quality (Formatting & Analysis)
# Define the absolute universe of sources for this repository
# Globbing is encouraged for CI quality gates to ensure 100% coverage
file(GLOB_RECURSE ALL_UNIVERSE_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/src/mock/*.c"
    "${PROJECT_SOURCE_DIR}/src/mock/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
    "${PROJECT_SOURCE_DIR}/tests/*.c"
    "${PROJECT_SOURCE_DIR}/tests/*.h"
)

include(${PROJECT_SOURCE_DIR}/caffeine-build/cmake/CaffeineMacros.cmake)
cfn_add_universe_targets(INPUTS ${ALL_UNIVERSE_SOURCES})

MESSAGE(STATUS "Target CAFFEINE_BUILD_STAGE: ${CAFFEINE_BUILD_STAGE}")


#################################### Build ####################################
# Create our target, this is a Header-Defined Interface library so we should use interface
add_library(${PROJECT_NAME} INTERFACE)
add_library(caffeine::hal ALIAS ${PROJECT_NAME})

# Consistent naming for exported target
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME hal)

# Pass the version down to the code using a compiler definition
target_compile_definitions(${PROJECT_NAME} INTERFACE CAFFEINE_HAL_VERSION="${PROJECT_VERSION}")

# The minimum required C standard
target_compile_features(${PROJECT_NAME} INTERFACE c_std_11)

# --- Modern Header Management (CMake 3.23+) ---
# Actual binary targets MUST explicitly list all headers for determinism
set(PROJECT_ALL_HEADERS
    include/cfn_hal.h
    include/cfn_hal_adc.h
    include/cfn_hal_base.h
    include/cfn_hal_base_impl.h
    include/cfn_hal_can.h
    include/cfn_hal_clock.h
    include/cfn_hal_comp.h
    include/cfn_hal_crypto.h
    include/cfn_hal_dac.h
    include/cfn_hal_dma.h
    include/cfn_hal_eth.h
    include/cfn_hal_gpio.h
    include/cfn_hal_i2c.h
    include/cfn_hal_i2s.h
    include/cfn_hal_irq.h
    include/cfn_hal_nvm.h
    include/cfn_hal_pwm.h
    include/cfn_hal_qspi.h
    include/cfn_hal_rtc.h
    include/cfn_hal_sdio.h
    include/cfn_hal_spi.h
    include/cfn_hal_timer.h
    include/cfn_hal_types.h
    include/cfn_hal_uart.h
    include/cfn_hal_usb.h
    include/cfn_hal_wdt.h
    include/cfn_hal_util.h
)

# We use FILE_SET to define public headers. This automatically:
# 1. Handles include directories for both build and install.
# 2. Ensures IDEs like CLion index the headers for syntax highlighting.
# 3. Simplifies the installation process.
target_sources(
        ${PROJECT_NAME}
        INTERFACE
        FILE_SET headers
        TYPE HEADERS
        BASE_DIRS include
        FILES ${PROJECT_ALL_HEADERS}
)

# Define an option to build tests globally
option(CFN_BUILD_TESTS "Build unit tests" OFF)

if(CFN_BUILD_TESTS)
    # Automatically expose the mock target if tests are enabled anywhere in the workspace
    # Static libraries are binary targets - MUST be explicit
    set(MOCK_SOURCES
        src/mock/cfn_hal_adc_port.c
        src/mock/cfn_hal_can_port.c
        src/mock/cfn_hal_clock_port.c
        src/mock/cfn_hal_comp_port.c
        src/mock/cfn_hal_crypto_port.c
        src/mock/cfn_hal_dac_port.c
        src/mock/cfn_hal_dma_port.c
        src/mock/cfn_hal_eth_port.c
        src/mock/cfn_hal_gpio_port.c
        src/mock/cfn_hal_i2c_port.c
        src/mock/cfn_hal_i2s_port.c
        src/mock/cfn_hal_irq_port.c
        src/mock/cfn_hal_nvm_port.c
        src/mock/cfn_hal_pwm_port.c
        src/mock/cfn_hal_qspi_port.c
        src/mock/cfn_hal_rtc_port.c
        src/mock/cfn_hal_sdio_port.c
        src/mock/cfn_hal_spi_port.c
        src/mock/cfn_hal_timer_port.c
        src/mock/cfn_hal_uart_port.c
        src/mock/cfn_hal_usb_port.c
        src/mock/cfn_hal_wdt_port.c
    )
    if(MOCK_SOURCES)
        add_library(caffeine-hal-mock STATIC ${MOCK_SOURCES})
        target_link_libraries(caffeine-hal-mock PUBLIC ${PROJECT_NAME})
        add_library(caffeine::hal-mock ALIAS caffeine-hal-mock)
    endif()
endif()

if(CFN_BUILD_TESTS)
    if(PROJECT_IS_TOP_LEVEL)
        include(CTest)
        if(BUILD_TESTING)
            add_subdirectory(tests)
        endif()
    endif()
endif()

#################################### Installation ####################################

# Prepare the target data that should be exported.
# Note: FILE_SET headers are automatically installed when we include the FILE_SET in the install command.
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Prepare the version information
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

# Configure the input and output of the config file that will be generated
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Store the ${PROJECT_NAME}_Targets generated previously in the config file
install(
        EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE caffeine::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Copy the generic config file and the version config file to the installation destination
install(
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# ==============================================================================
# Tooling: Formatting & Analysis
# ==============================================================================

# Use globbing for quality gate sources
file(GLOB_RECURSE ALL_ANALYSIS_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/src/mock/*.c"
    "${PROJECT_SOURCE_DIR}/src/mock/*.h"
)

cfn_add_code_quality_targets(
    ${PROJECT_NAME}
    HEADERS_ONLY
    ANALYSIS_SOURCES ${ALL_ANALYSIS_SOURCES}
    CPPCHECK_ARGS -I ${PROJECT_SOURCE_DIR}/include
    TIDY_ARGS -I${PROJECT_SOURCE_DIR}/include
    TIDY_HEADER_FILTER ".*"
)

# 4. C11 Compliance Check
# This target compiles a minimal C file that includes all HAL headers
# using strict C11 flags to ensure no non-standard extensions are used.
if(NOT CMAKE_CROSSCOMPILING)
    add_executable(${PROJECT_NAME}-compliance ${PROJECT_SOURCE_DIR}/tests/compliance.c)
    target_link_libraries(${PROJECT_NAME}-compliance PRIVATE ${PROJECT_NAME})
    target_compile_options(${PROJECT_NAME}-compliance PRIVATE
        -std=c11
        -pedantic-errors
        -Wall
        -Wextra
        -Werror
    )
    set_target_properties(${PROJECT_NAME}-compliance PROPERTIES EXCLUDE_FROM_ALL ON)

    if(TARGET ${PROJECT_NAME}-analyze)
        add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-compliance)
    endif()
endif()

# 6. Doxygen Documentation
# Filter out tests from universe for docs
set(ALL_UNIVERSE_SOURCES_NO_TESTS ${ALL_UNIVERSE_SOURCES})
list(FILTER ALL_UNIVERSE_SOURCES_NO_TESTS EXCLUDE REGEX ".*/tests/.*")
cfn_add_docs(
    ${PROJECT_NAME}-docs
    INPUTS ${ALL_UNIVERSE_SOURCES_NO_TESTS} ${PROJECT_SOURCE_DIR}/README.md
    COMMENT "Generating API documentation with Doxygen"
)
