#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <variant>
#include <string_view>

namespace protocol
{
    inline constexpr std::size_t HEADER_SIZE = 10;

    struct Header
    {
        std::uint8_t type{};
        std::uint32_t sequence{};
        std::uint16_t payloadLength{};
    };

    std::array<std::byte, HEADER_SIZE>
        EncodeHeader(const Header& header);

    std::optional<Header>
        DecodeHeader(std::span<const std::byte> bytes);

    enum class DecodeError
    {
        InvalidSize,
        InvalidMagic,
        UnsupportedVersion
    };

    using HeaderDecodeResult =
        std::variant<Header, DecodeError>;

    HeaderDecodeResult DecodeHeaderDetailed(
        std::span<const std::byte> bytes);

    std::string_view DecodeErrorMessage(
        DecodeError error) noexcept;
}
