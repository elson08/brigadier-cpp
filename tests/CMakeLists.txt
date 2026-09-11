include(FetchContent)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(brigadier_tests brigadier_tests.cpp)
target_link_libraries(brigadier_tests PRIVATE brigadier::brigadier GTest::gtest_main)

if(MSVC)
    target_compile_options(brigadier_tests PRIVATE /W4 $<$<BOOL:${BRIGADIER_WARNINGS_AS_ERRORS}>:/WX>)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(brigadier_tests PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${BRIGADIER_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

include(GoogleTest)
gtest_discover_tests(brigadier_tests)
