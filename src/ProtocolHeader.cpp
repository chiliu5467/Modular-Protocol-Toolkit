#include "protocol/ProtocolHeader.h"
#include "ByteOrder.h"
#include "protocol/HeaderPolicy.h"

#include <algorithm>

namespace protocol
{
    // =====================================
    // 1. Encode Header
    // =====================================

    std::array<std::byte, HEADER_SIZE>
        EncodeHeader(const Header& header)
    {
        std::array<std::byte, HEADER_SIZE> result{};
        size_t offset = 0;

        result[offset++] = std::byte{ 0x4D };
        result[offset++] = std::byte{ 0x4C };
        result[offset++] = std::byte{ 0x01 };
		result[offset++] = static_cast<std::byte>(header.type);

		auto sequenceBytes = protocol::detail::EncodeU32BE(header.sequence);
        std::copy(sequenceBytes.begin(), sequenceBytes.end(), result.begin() + offset);
        offset += sequenceBytes.size();

		auto payloadLengthBytes = protocol::detail::EncodeU16BE(header.payloadLength);
        std::copy(payloadLengthBytes.begin(), payloadLengthBytes.end(), result.begin() + offset);
        offset += payloadLengthBytes.size();

		return result;
    }

    // =====================================
    // 2. Core Decoder
    // =====================================

    HeaderDecodeResult DecodeHeaderDetailed(
        std::span<const std::byte> bytes)
    {
        Header header{};

        if (bytes.size() != HEADER_SIZE)
        {
            return DecodeError::InvalidSize;
        }

        if (bytes[0] != std::byte{ 0x4D } ||
            bytes[1] != std::byte{ 0x4C })
        {
            return DecodeError::InvalidMagic;
        }

        if (bytes[2] != std::byte{ 0x01 })
        {
            return DecodeError::UnsupportedVersion;
        }

        header.type = std::to_integer<std::uint8_t>(bytes[3]);

        auto sequenceBytes = bytes.subspan<4, 4>();
        header.sequence = protocol::detail::DecodeU32BE(sequenceBytes);

        auto payloadLengthBytes = bytes.subspan<8, 2>();
        header.payloadLength = protocol::detail::DecodeU16BE(payloadLengthBytes);

        return header;
    }

    // =====================================
    // 3. Simple Decoder
    // =====================================

    std::optional<Header>
        DecodeHeader(std::span<const std::byte> bytes)
    {
		auto result = DecodeHeaderDetailed(bytes);

        if (auto* header = std::get_if<Header>(&result))
        {
            return *header;
        }

        return std::nullopt;
    }

    // =====================================
    // 4. Policy-based Decoder
    // =====================================

    HeaderDecodeResult DecodeHeaderWithPolicy(
        std::span<const std::byte> bytes,
        const HeaderPolicy& policy)
    {
		auto result = DecodeHeaderDetailed(bytes);

		const auto* header = std::get_if<Header>(&result);

        if (header == nullptr)
        {
			return result;
        }

        if (!policy.Accept(*header))
        {
			return DecodeError::PolicyRejected;
        }

        return *header;
    }

    // =====================================
    // 5. Error Message
    // =====================================

    std::string_view DecodeErrorMessage(
        DecodeError error) noexcept
    {
        switch (error)
        {
        case DecodeError::InvalidSize:
            return "Invalid header size";

        case DecodeError::InvalidMagic:
            return "Invalid magic bytes";

        case DecodeError::UnsupportedVersion:
            return "Unsupported protocol version";

        case DecodeError::PolicyRejected:
            return "Header rejected by policy";
        }

        return "Unknown decode error";
    }
}