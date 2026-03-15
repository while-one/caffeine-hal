#################################### Config ####################################

# Minimum required version updated to 3.23 to support FILE_SET
cmake_minimum_required(VERSION 3.23)

# Information about the project
project(
        caffeine-hal
        VERSION 0.3.3
        LANGUAGES C
        DESCRIPTION "A Header-Defined Interface c library, it provides the hal layer for the Caffeine framework"
        HOMEPAGE_URL "https://github.com/while-one/caffeine-hal"
)

# This provides some important cross-platform paths to be used later during install
include(GNUInstallDirs)

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
# Gather all headers for target definition and tooling
file(GLOB PROJECT_ALL_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

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

# Define an option to build tests, default is OFF unless this is the top-level project
if(PROJECT_IS_TOP_LEVEL)
    set(DEFAULT_BUILD_TESTS ON)
else()
    set(DEFAULT_BUILD_TESTS OFF)
endif()
option(CAFFEINE_HAL_BUILD_TESTS "Build the Caffeine-HAL test executable" ${DEFAULT_BUILD_TESTS})

if(CAFFEINE_HAL_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
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

# Gather sources for formatting and analysis
set(ALL_HEADERS ${PROJECT_ALL_HEADERS})
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
set(ALL_ANALYSIS_SOURCES ${ALL_HEADERS} ${TEST_SOURCES})


# 1. Code Formatting (clang-format)
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    option(FORMAT_DRY_RUN "Run clang-format in dry-run mode" OFF)
    
    if(FORMAT_DRY_RUN)
        set(FORMAT_ARGS --dry-run --Werror)
        set(FORMAT_COMMENT "Checking header formatting with clang-format (BARR-C style)...")
    else()
        set(FORMAT_ARGS -i)
        set(FORMAT_COMMENT "Formatting header files with clang-format (BARR-C style)...")
    endif()

    add_custom_target(
        ${PROJECT_NAME}-format
        COMMAND ${CLANG_FORMAT}
        ${FORMAT_ARGS}
        -style=file:${PROJECT_SOURCE_DIR}/config/coding/.clang-format
        ${ALL_ANALYSIS_SOURCES}
        COMMENT "${FORMAT_COMMENT}"
    )
else()
    message(WARNING "clang-format not found.")
endif()

# 2. Static Analysis (cppcheck)
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(
        ${PROJECT_NAME}-cppcheck
        COMMAND ${CPPCHECK}
        --enable=all
        --suppress=missingIncludeSystem
        --suppress=unusedFunction
        --inline-suppr
        --std=c11
        -I ${PROJECT_SOURCE_DIR}/include
        ${ALL_HEADERS}
        COMMENT "Running cppcheck static analysis..."
    )
    
    if(NOT TARGET ${PROJECT_NAME}-analyze)
        add_custom_target(${PROJECT_NAME}-analyze COMMENT "Running full static analysis suite...")
    endif()
    add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-cppcheck)
endif()

# 3. Static Analysis (clang-tidy)
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    add_custom_target(
        ${PROJECT_NAME}-tidy
        COMMAND ${CLANG_TIDY}
        --config-file=${PROJECT_SOURCE_DIR}/config/coding/.clang-tidy
        --header-filter=.*
        ${ALL_HEADERS}
        ${PROJECT_SOURCE_DIR}/tests/compliance.c
        --
        -I${PROJECT_SOURCE_DIR}/include
        -std=c11
        COMMENT "Running clang-tidy static analysis..."
    )
    
    if(TARGET ${PROJECT_NAME}-analyze)
        add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-tidy)
    endif()
endif()

# 4. C11 Compliance Check
# This target compiles a minimal C file that includes all HAL headers
# using strict C11 flags to ensure no non-standard extensions are used.
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

# 5. Doxygen Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "Caffeine-HAL")
    set(DOXYGEN_WARN_AS_ERROR YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
    set(DOXYGEN_EXTRACT_ALL YES)

    doxygen_add_docs(
        ${PROJECT_NAME}-docs
        ${PROJECT_SOURCE_DIR}/include
        COMMENT "Generating API documentation with Doxygen"
    )
else()
    message(WARNING "Doxygen not found - documentation cannot be generated")
endif()

