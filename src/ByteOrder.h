#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace protocol::detail
{
    std::array<std::byte, 4>
        EncodeU32BE(std::uint32_t value);

    std::uint32_t
        DecodeU32BE(std::span<const std::byte, 4> bytes);

    std::array<std::byte, 2>
        EncodeU16BE(std::uint16_t value);

    std::uint16_t
        DecodeU16BE(std::span<const std::byte, 2> bytes);
}