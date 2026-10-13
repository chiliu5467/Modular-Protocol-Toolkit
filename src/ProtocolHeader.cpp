#include "protocol/ProtocolHeader.h"
#include "ByteOrder.h"

namespace protocol
{
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

    std::optional<Header>
        DecodeHeader(std::span<const std::byte> bytes)
    {
        Header header{};

        if (bytes.size() != HEADER_SIZE)
        {
            return std::nullopt;
        }

        if (bytes[0] != std::byte{ 0x4D } ||
            bytes[1] != std::byte{ 0x4C } ||
            bytes[2] != std::byte{ 0x01 })
        {
			return std::nullopt;
        }

		header.type = std::to_integer<std::uint8_t>(bytes[3]);

        auto sequenceBytes = bytes.subspan<4, 4>();
		header.sequence = protocol::detail::DecodeU32BE(sequenceBytes);

		auto payloadLengthBytes = bytes.subspan<8, 2>();
		header.payloadLength = protocol::detail::DecodeU16BE(payloadLengthBytes);

        return header;
    }
}