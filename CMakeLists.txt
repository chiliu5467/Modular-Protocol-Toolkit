# CMakeList.txt: 最上層的 CMake 專案檔，執行全域設定
# 並在這裡包含子專案。
#
cmake_minimum_required (VERSION 3.20)

# 如果支援，則為 MSVC 編譯器啟用熱重新載入。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project settings
project(ModularProtocolToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static Library
add_library(protocol STATIC
    src/ProtocolHeader.cpp
    src/ByteOrder.cpp
    src/BasicHeaderPolicy.cpp
)

target_compile_features(protocol
    PUBLIC cxx_std_20
)

# Public header directory
target_include_directories(protocol 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# CLI executable
add_executable(protocol_cli
    app/main.cpp
 )

target_link_libraries(protocol_cli 
    PRIVATE protocol)

# Test executable
enable_testing()

add_executable(protocol_tests
    tests/test_protocol.cpp
    tests/header_self_contained.cpp
)

target_link_libraries(protocol_tests PRIVATE protocol)

add_test(NAME protocol_smoke COMMAND protocol_tests)

add_executable(policy_tests
    tests/test_policy.cpp
)

target_link_libraries(policy_tests
    PRIVATE protocol
)

add_test(
    NAME policy_tests
    COMMAND policy_tests
)
