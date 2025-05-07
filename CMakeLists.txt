cmake_minimum_required(VERSION 3.11)
cmake_policy(SET CMP0074 NEW)      # Use <packageName>_ROOT variable in find_package()
cmake_policy(SET CMP0077 NEW)      # option() honors normal variable

project(certify VERSION 0.1 LANGUAGES CXX)

set(BOOST_INCLUDE_LIBRARIES asio beast endian)

get_directory_property(CERTIFY_HAS_PARENT PARENT_DIRECTORY)
if(BOOST_SUPERPROJECT_SOURCE_DIR)
    message(VERBOSE "certify library is part of Boost superproject")
elseif(CERTIFY_HAS_PARENT)
    message(STATUS "certify library is built within the project")
else()
    set(BOOST_USE_VERSION "1.88.0" CACHE STRING "Boost version number to use for fetching sources")
    set(BOOST_USE_SUFFIX "-cmake.tar.xz" CACHE STRING "Boost source archive suffix")
    set(BOOST_DOWNLOAD_URL "https://github.com/boostorg/boost/releases/download")
    set(BOOST_SOURCE_URL "${BOOST_DOWNLOAD_URL}/boost-${BOOST_USE_VERSION}/boost-${BOOST_USE_VERSION}${BOOST_USE_SUFFIX}")

    list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/CMakeModules)
    include(FetchContent)
    FetchContent_Declare(Boost
        URL ${BOOST_SOURCE_URL}
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        FIND_PACKAGE_ARGS 1.67.0
    )
    FetchContent_MakeAvailable(Boost)
endif()

add_compile_definitions(BOOST_ERROR_CODE_HEADER_ONLY BOOST_REGEX_NO_LIB BOOST_DATE_TIME_NO_LIB)

find_package(Threads)
find_package(OpenSSL REQUIRED)

add_library(certify_core INTERFACE)

add_library(certify::core ALIAS certify_core)

target_compile_features(certify_core INTERFACE cxx_std_17)

target_include_directories(certify_core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_link_libraries(certify_core INTERFACE Crypt32.lib)
endif()

target_link_libraries(certify_core
    INTERFACE
        $<BUILD_INTERFACE:Boost::asio>
        $<BUILD_INTERFACE:Boost::beast>
        $<BUILD_INTERFACE:Boost::endian>
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
