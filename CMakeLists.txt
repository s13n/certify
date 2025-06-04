cmake_minimum_required(VERSION 3.24)

project(certify VERSION 0.2 LANGUAGES CXX)

set(BOOST_INCLUDE_LIBRARIES asio beast endian)

get_directory_property(CERTIFY_HAS_PARENT PARENT_DIRECTORY)
if(BOOST_SUPERPROJECT_SOURCE_DIR)
    message(VERBOSE "certify library is part of Boost superproject")
elseif(CERTIFY_HAS_PARENT)
    message(STATUS "certify library is built within the project")
else()
    message(STATUS "trying to find a usable installed version of Boost")
    find_package(Boost 1.67.0)
endif()

add_compile_definitions(BOOST_ERROR_CODE_HEADER_ONLY BOOST_REGEX_NO_LIB BOOST_DATE_TIME_NO_LIB)

find_package(Threads)
find_package(OpenSSL REQUIRED)

add_library(certify_core INTERFACE)

add_library(certify::core ALIAS certify_core)

target_compile_features(certify_core INTERFACE cxx_std_17)

target_include_directories(certify_core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_link_libraries(certify_core INTERFACE Crypt32.lib)
endif()

target_link_libraries(certify_core
    INTERFACE
        $<BUILD_INTERFACE:OpenSSL::SSL>
        $<BUILD_INTERFACE:OpenSSL::Crypto>
        $<BUILD_INTERFACE:Threads::Threads>
)

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "certifyConfigVersion.cmake"
        COMPATIBILITY AnyNewerVersion
    )
    install(FILES
            "certifyConfig.cmake"
            "${CMAKE_BINARY_DIR}/certifyConfigVersion.cmake"
        DESTINATION lib/cmake/certify
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.ipp"
    )
    install(TARGETS certify_core
        EXPORT certifyTargets
        INCLUDES DESTINATION include
    )
    install(EXPORT certifyTargets
        FILE certifyTargets.cmake
        NAMESPACE certify::
        DESTINATION lib/cmake/certify
    )
endif()

include(CTest)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
