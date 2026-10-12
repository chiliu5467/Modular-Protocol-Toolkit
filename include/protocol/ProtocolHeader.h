#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace protocol
{
    struct Header
    {
        std::uint8_t type{};
        std::uint32_t sequence{};
        std::uint16_t payloadLength{};
    };

    std::array<std::byte, 10>
        EncodeHeader(const Header& header);
}
