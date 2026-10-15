#include "protocol/ProtocolHeader.h"
#include "protocol/HeaderPolicy.h"
#include "protocol/BasicHeaderPolicy.h"

#include <cassert>
#include <variant>

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

int main()
{
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

    assert(std::holds_alternative<protocol::Header>(accepted));

    // Test 2: RejectAllPolicy
    RejectAllPolicy rejectPolicy;

    auto rejected = protocol::DecodeHeaderWithPolicy(
        bytes, rejectPolicy
    );

    assert(
        std::get<protocol::DecodeError>(rejected) ==
        protocol::DecodeError::PolicyRejected
    );

    // Test 3: BasicHeaderPolicy
    protocol::BasicHeaderPolicy basicPolicy;

    auto basicResult = protocol::DecodeHeaderWithPolicy(
        bytes, basicPolicy
    );

    assert(std::holds_alternative<protocol::Header>(basicResult));

    return 0;
}