#include "protocol/ProtocolHeader.h"
#include "protocol/HeaderPolicy.h"
#include "protocol/BasicHeaderPolicy.h"

#include <cassert>
#include <variant>
#include <iostream>
#include <string_view>

class RejectAllPolicy : public protocol::HeaderPolicy
{
public:
    bool Accept(const protocol::Header&) const override
    {
        return false;
    }
};

class AcceptAllPolicy : public protocol::HeaderPolicy
{
public:
    bool Accept(const protocol::Header&) const override
    {
        return true;
    }
};

class CountingPolicy : public protocol::HeaderPolicy
{
public:
    mutable int callCount = 0;

    bool Accept(const protocol::Header&) const override
    {
        ++callCount;
        return true;
    }
};

bool TestBasicPolicy(
    std::uint8_t type,
    std::uint16_t payloadLength,
    bool expectedAccept)
{
    protocol::BasicHeaderPolicy policy;

    protocol::Header header{
        type,
        0x12345678,
        payloadLength
    };

    auto bytes = protocol::EncodeHeader(header);

    auto result = protocol::DecodeHeaderWithPolicy(
        bytes,
        policy
    );

    bool accepted =
        std::holds_alternative<protocol::Header>(result);

    return accepted == expectedAccept;
}

bool Check(bool condition, std::string_view testName)
{
    if (condition)
    {
        std::cout << "[PASS] " << testName << '\n';
        return true;
    }

    std::cerr << "[FAIL] " << testName << '\n';
    return false;
}

int main()
{
    int failed = 0;

    // Prepare a valid header
    protocol::Header header{
        1,
        0x12345678,
        100
    };

    auto bytes = protocol::EncodeHeader(header);

    // Test 1: AcceptAllPolicy
    AcceptAllPolicy acceptPolicy;

    auto accepted = protocol::DecodeHeaderWithPolicy(
        bytes, acceptPolicy
    );

    if (!Check(
        std::holds_alternative<protocol::Header>(accepted),
        "AcceptAllPolicy"
    ))
    {
        ++failed;
    }

    // Test 2: RejectAllPolicy
    RejectAllPolicy rejectPolicy;

    auto rejected = protocol::DecodeHeaderWithPolicy(
        bytes, rejectPolicy
    );

    const auto* error =
        std::get_if<protocol::DecodeError>(&rejected);

    if (!Check(
        error != nullptr &&
        *error == protocol::DecodeError::PolicyRejected,
        "RejectAllPolicy"
    ))
    {
        ++failed;
    }

    // Test 3: BasicHeaderPolicy
    protocol::BasicHeaderPolicy basicPolicy;

    auto basicResult = protocol::DecodeHeaderWithPolicy(
        bytes, basicPolicy
    );

    if (!Check(
        std::holds_alternative<protocol::Header>(basicResult),
        "BasicHeaderPolicy"
    ))
    {
        ++failed;
    }

    // Test 4: CountingPolicy
    auto invalidBytes = bytes;
    invalidBytes[0] = std::byte{ 0x00 };

    CountingPolicy policy;

    auto result = protocol::DecodeHeaderWithPolicy(
        invalidBytes,
        policy
    );

    const auto* error2 =
        std::get_if<protocol::DecodeError>(&result);

    if (!Check(
        error2 != nullptr &&
        *error2 == protocol::DecodeError::InvalidMagic,
        "InvalidMagicNumber"
    ))
    {
        ++failed;
    }

    if (!Check(
        policy.callCount == 0,
        "CountingPolicy"
    ))
    {
        ++failed;
    }

    if (!TestBasicPolicy(1, 100, true)) ++failed;
    if (!TestBasicPolicy(2, 1024, true)) ++failed;
    if (!TestBasicPolicy(3, 100, false)) ++failed;
    if (!TestBasicPolicy(1, 1025, false)) ++failed;
    if (!TestBasicPolicy(2, 0, true)) ++failed;
    if (!TestBasicPolicy(0, 0, false)) ++failed;

    std::cout << "\nFailed tests: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}