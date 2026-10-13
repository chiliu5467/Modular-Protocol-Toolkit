#include <array>
#include <cassert>
#include <cstddef>

#include "protocol/ProtocolHeader.h"

int main()
{
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

    if (encoded != expected)
    {
        return 1;
    }


    // Test 2: Decode Header
    auto decoded = protocol::DecodeHeader(encoded);

    if (!decoded ||
        decoded->type != header.type ||
        decoded->sequence != header.sequence ||
        decoded->payloadLength != header.payloadLength)
    {
        return 2;
    }

    // Test 3: Invalid Magic Bytes
    auto invalidMagic = encoded;
    invalidMagic[0] = std::byte{ 0x00 };

    if (protocol::DecodeHeader(invalidMagic))
    {
        return 3;
    }

    // Test 4: Invalid Version
    auto invalidVersion = encoded;
    invalidVersion[2] = std::byte{ 0xFF };

    if (protocol::DecodeHeader(invalidVersion))
    {
        return 4;
    }

    // Test 5: Truncated Header
    std::array<std::byte, 5> truncated{};

    if (protocol::DecodeHeader(truncated))
    {
        return 5;
    }

    // Test 6: Extra Bytes
    std::array<std::byte, 11> oversized{};

    if (protocol::DecodeHeader(oversized))
    {
        return 6;
    }


    return 0;
}