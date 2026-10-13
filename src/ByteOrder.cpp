#include "ByteOrder.h"

namespace protocol::detail
{
    std::array<std::byte, 4>
        EncodeU32BE(std::uint32_t value)
    {
        return {
        static_cast<std::byte>((value >> 24) & 0xFF),
        static_cast<std::byte>((value >> 16) & 0xFF),
        static_cast<std::byte>((value >> 8) & 0xFF),
        static_cast<std::byte>(value & 0xFF)
        };
    }

    std::uint32_t
        DecodeU32BE(std::span<const std::byte, 4> bytes)
    {
        return (std::to_integer<std::uint32_t>(bytes[0]) << 24 |
            std::to_integer<std::uint32_t>(bytes[1]) << 16 |
            std::to_integer<std::uint32_t>(bytes[2]) << 8 |
            std::to_integer<std::uint32_t>(bytes[3]));
    }

    std::array<std::byte, 2>
        EncodeU16BE(std::uint16_t value)
    {
        return {
        static_cast<std::byte>((value >> 8) & 0xFF),
        static_cast<std::byte>(value & 0xFF)
        };
    }

    std::uint16_t
        DecodeU16BE(std::span<const std::byte, 2> bytes)
    {
        return (std::to_integer<std::uint16_t>(bytes[0]) << 8 |
            std::to_integer<std::uint16_t>(bytes[1]));
    }
}