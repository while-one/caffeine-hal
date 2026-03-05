enable_language(CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# Prevent GTest from overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create the test executable
file(GLOB TEST_SOURCES "*.cpp")
add_executable(${PROJECT_NAME}-test ${TEST_SOURCES})

# Link your HAL library and GoogleTest
target_link_libraries(${PROJECT_NAME}-test
    PRIVATE
    ${PROJECT_NAME}
    GTest::gtest_main
)

# Tell CTest to discover and run all tests in this executable
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}-test)