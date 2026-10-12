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

    assert(encoded == expected);

    return 0;
}