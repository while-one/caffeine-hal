#################################### Config ####################################

# The minimum required version of CMake, not doing anything fancy to let us keep it low
cmake_minimum_required(VERSION 3.12)

# Information about the project
project(
        caffeine-hal
        VERSION 0.1.2
        LANGUAGES C
        DESCRIPTION "A header-only c library, it provides the hal layer for the Caffeine framework"
        HOMEPAGE_URL "https://github.com/while-one/caffeine-hal"
)

# This provides some important cross-platform paths to be used later during install
include(GNUInstallDirs)

#################################### Build ####################################
# Create our target, this is a header-only library so we should use interface
add_library(${PROJECT_NAME} INTERFACE)

# Pass the version down to the code using a compiler definition
target_compile_definitions(${PROJECT_NAME} INTERFACE CAFFEINE_HAL_VERSION="${PROJECT_VERSION}")

# The minimum required C standard
target_compile_features(${PROJECT_NAME} INTERFACE c_std_11)

# The include directories, of course they are different for build and install
target_include_directories(
        ${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Define an option to build tests, default is OFF unless this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
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

# Prepare the target data that should be exported
# Store it in ${PROJECT_NAME}_Targets
# Assign the destination directories, just use the ones provided by GNUInstallDirs
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
)

# Prepare the version information
# Store it in "${PROJECT_NAME}ConfigVersion.cmake"
# The version is the same as the project version above passed to project function
# Here specifying that compatibility will be kept across other version with same major version
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

# Configure the input and output of the config file that will be generated
# Provide the installation path of this config file
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Store the ${PROJECT_NAME}_Targets generated previously in the config file
# This target belongs to our library with name space ${PROJECT_NAME}::
# Provide the installation path of this config file
install(
        EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Copy the generic config file and the version config file to the installation destination
install(
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Find all the header files inside the include directory
file(
        GLOB_RECURSE
        EXPORTED_INCLUDES
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/*.h"
)

# Copy the header files to the destination CMAKE_INSTALL_INCLUDEDIR
install(FILES ${EXPORTED_INCLUDES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Gather test sources for formatting and analysis
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
set(ALL_ANALYSIS_SOURCES ${EXPORTED_INCLUDES} ${TEST_SOURCES})

# ==============================================================================
# Code Formatting (clang-format)
# ==============================================================================
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    # Option to run clang-format in dry-run mode (useful for CI)
    option(FORMAT_DRY_RUN "Run clang-format in dry-run mode (fails if formatting is required)" OFF)
    
    if(FORMAT_DRY_RUN)
        set(FORMAT_ARGS --dry-run --Werror)
        set(FORMAT_COMMENT "Checking header formatting with clang-format (BARR-C style)...")
    else()
        set(FORMAT_ARGS -i)
        set(FORMAT_COMMENT "Formatting header files with clang-format (BARR-C style)...")
    endif()

    # Define formatting target
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT}
        ${FORMAT_ARGS}
        -style=file:${PROJECT_SOURCE_DIR}/config/coding/.clang-format
        ${ALL_ANALYSIS_SOURCES}
        COMMENT "${FORMAT_COMMENT}"
    )
else()
    message(WARNING "clang-format not found. 'format' target will not be available.")
endif()

# ==============================================================================
# Static Analysis (cppcheck)
# ==============================================================================
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    # Define standalone cppcheck target
    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK}
        --enable=all
        --suppress=missingIncludeSystem
        --suppress=unusedFunction
        --inline-suppr
        --std=c11
        -I ${PROJECT_SOURCE_DIR}/include
        ${EXPORTED_INCLUDES}
        COMMENT "Running cppcheck static analysis (all checks except unusedFunction)..."
    )
    
    # Create the top-level 'analyze' target that will wrap both tools
    if(NOT TARGET analyze)
        add_custom_target(analyze COMMENT "Running full static analysis suite...")
    endif()
    add_dependencies(analyze cppcheck)
else()
    message(WARNING "cppcheck not found. 'cppcheck' target will not be available.")
endif()

# ==============================================================================
# Static Analysis (clang-tidy)
# ==============================================================================
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    # Define clang-tidy target
    add_custom_target(
        tidy
        COMMAND ${CLANG_TIDY}
        --config-file=${PROJECT_SOURCE_DIR}/config/coding/.clang-tidy
        --header-filter=.*
        ${EXPORTED_INCLUDES}
        --
        -I${PROJECT_SOURCE_DIR}/include
        COMMENT "Running clang-tidy static analysis..."
    )
    
    # Make 'analyze' run both cppcheck and clang-tidy if both exist
    if(TARGET analyze)
        add_dependencies(analyze tidy)
    endif()
else()
    message(WARNING "clang-tidy not found. 'tidy' target will not be available.")
endif()
