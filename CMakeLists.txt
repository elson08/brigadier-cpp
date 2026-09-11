cmake_minimum_required(VERSION 3.23)

project(brigadier
    VERSION 0.1.0
    DESCRIPTION "A constexpr C++23 command parser and dispatcher"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BRIGADIER_BUILD_TESTS "Build the brigadier test suite" ${PROJECT_IS_TOP_LEVEL})
option(BRIGADIER_WARNINGS_AS_ERRORS "Treat test warnings as errors" ${PROJECT_IS_TOP_LEVEL})

file(GLOB_RECURSE BRIGADIER_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(brigadier INTERFACE ${BRIGADIER_HEADERS})
add_library(brigadier::brigadier ALIAS brigadier)

target_compile_features(brigadier INTERFACE cxx_std_23)
target_include_directories(brigadier INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_sources(brigadier INTERFACE
    FILE_SET headers
    TYPE HEADERS
    BASE_DIRS include
    FILES ${BRIGADIER_HEADERS}
)

install(
    TARGETS brigadier
    EXPORT brigadierTargets
    FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    EXPORT brigadierTargets
    FILE brigadierTargets.cmake
    NAMESPACE brigadier::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brigadier
)

configure_package_config_file(
    cmake/brigadierConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/brigadierConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brigadier
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/brigadierConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/brigadierConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/brigadierConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brigadier
)

if(BRIGADIER_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()
