
#include "protocol/ProtocolHeader.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <string_view>
#include <variant>

// --------------------------------------------------
// Test Helpers
// --------------------------------------------------

bool TestInvalidInput(
    std::string_view testName,
    std::span<const std::byte> bytes,
    protocol::DecodeError expectedError)
{
    // Verify legacy API
    if (protocol::DecodeHeader(bytes).has_value())
    {
        std::cerr << "[FAIL] " << testName
            << " - Legacy API accepted invalid input\n";
        return false;
    }

    // Verify detailed API
    auto result = protocol::DecodeHeaderDetailed(bytes);

    auto* error =
        std::get_if<protocol::DecodeError>(&result);

    if (!error)
    {
        std::cerr << "[FAIL] " << testName
            << " - Detailed API returned Header\n";
        return false;
    }

    if (*error != expectedError)
    {
        std::cerr << "[FAIL] " << testName
            << " - Unexpected error: "
            << protocol::DecodeErrorMessage(*error)
            << '\n';
        return false;
    }

    std::cout << "[PASS] " << testName
        << " - "
        << protocol::DecodeErrorMessage(*error)
        << '\n';

    return true;
}

bool TestErrorMessage(
    std::string_view testName,
    protocol::DecodeError error,
    std::string_view expectedMessage)
{
    auto actualMessage =
        protocol::DecodeErrorMessage(error);

    if (actualMessage != expectedMessage)
    {
        std::cerr << "[FAIL] " << testName
            << " - Expected: " << expectedMessage
            << ", Actual: " << actualMessage
            << '\n';
        return false;
    }

    std::cout << "[PASS] " << testName << '\n';
    return true;
}

// --------------------------------------------------
// Main Test Runner
// --------------------------------------------------

int main()
{
    int passed = 0;
    int failed = 0;

    // Record test results without stopping early
    auto RunTest = [&](bool success)
        {
            if (success)
                ++passed;
            else
                ++failed;
        };

    protocol::Header header{
        2,
        0x12345678,
        3
    };

    auto encoded = protocol::EncodeHeader(header);

    const std::array<std::byte, 10> expected{
        std::byte{0x4D},
        std::byte{0x4C},
        std::byte{0x01},
        std::byte{0x02},
        std::byte{0x12},
        std::byte{0x34},
        std::byte{0x56},
        std::byte{0x78},
        std::byte{0x00},
        std::byte{0x03}
    };

    // Test 1: Encode Header
    {
        bool success = (encoded == expected);

        std::cout << (success ? "[PASS] " : "[FAIL] ")
            << "Encode Header\n";

        RunTest(success);
    }

    // Test 2: Decode Header (Legacy API)
    {
        auto decoded = protocol::DecodeHeader(encoded);

        bool success =
            decoded.has_value() &&
            decoded->type == header.type &&
            decoded->sequence == header.sequence &&
            decoded->payloadLength == header.payloadLength;

        std::cout << (success ? "[PASS] " : "[FAIL] ")
            << "Decode Header (Legacy)\n";

        RunTest(success);
    }

    // Test 3: Decode Header (Detailed API)
    {
        auto result =
            protocol::DecodeHeaderDetailed(encoded);

        auto* decoded =
            std::get_if<protocol::Header>(&result);

        bool success =
            decoded != nullptr &&
            decoded->type == header.type &&
            decoded->sequence == header.sequence &&
            decoded->payloadLength == header.payloadLength;

        std::cout << (success ? "[PASS] " : "[FAIL] ")
            << "Decode Header (Detailed)\n";

        RunTest(success);
    }

    // --------------------------------------------------
    // Invalid Input Tests
    // --------------------------------------------------

    // Test 4: Invalid Magic
    auto invalidMagic = encoded;
    invalidMagic[0] = std::byte{ 0x00 };

    RunTest(TestInvalidInput(
        "Invalid Magic",
        invalidMagic,
        protocol::DecodeError::InvalidMagic
    ));

    // Test 5: Unsupported Version
    auto invalidVersion = encoded;
    invalidVersion[2] = std::byte{ 0xFF };

    RunTest(TestInvalidInput(
        "Unsupported Version",
        invalidVersion,
        protocol::DecodeError::UnsupportedVersion
    ));

    // Test 6: Truncated Header
    std::array<std::byte, 5> truncated{};

    RunTest(TestInvalidInput(
        "Truncated Header",
        truncated,
        protocol::DecodeError::InvalidSize
    ));

    // Test 7: Extra Bytes
    std::array<std::byte, 11> oversized{};

    RunTest(TestInvalidInput(
        "Extra Bytes",
        oversized,
        protocol::DecodeError::InvalidSize
    ));

    // Test 8: Empty Input
    std::array<std::byte, 0> empty{};

    RunTest(TestInvalidInput(
        "Empty Input",
        empty,
        protocol::DecodeError::InvalidSize
    ));

    // Test 9: Validation Priority
    std::array<std::byte, 5> multipleErrors{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0xFF},
        std::byte{0x00},
        std::byte{0x00}
    };

    RunTest(TestInvalidInput(
        "Validation Priority",
        multipleErrors,
        protocol::DecodeError::InvalidSize
    ));

    // --------------------------------------------------
    // Error Message Tests
    // --------------------------------------------------

    // Test 10
    RunTest(TestErrorMessage(
        "InvalidMagic Message",
        protocol::DecodeError::InvalidMagic,
        "Invalid magic bytes"
    ));

    // Test 11
    RunTest(TestErrorMessage(
        "UnsupportedVersion Message",
        protocol::DecodeError::UnsupportedVersion,
        "Unsupported protocol version"
    ));

    // Test 12
    RunTest(TestErrorMessage(
        "InvalidSize Message",
        protocol::DecodeError::InvalidSize,
        "Invalid header size"
    ));

    // --------------------------------------------------
    // Test Summary
    // --------------------------------------------------

    std::cout << "\n-----------------------------\n";
    std::cout << "Test Summary\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "Total:  " << passed + failed << '\n';
    std::cout << "-----------------------------\n";

    return failed == 0 ? 0 : 1;
}
