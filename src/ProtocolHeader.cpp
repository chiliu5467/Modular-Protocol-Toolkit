#include "protocol/ProtocolHeader.h"

namespace protocol
{
    inline constexpr std::size_t HEADER_SIZE = 10;

    std::array<std::byte, HEADER_SIZE>
        EncodeHeader(const Header& header)
    {
        std::array<std::byte, HEADER_SIZE> result{};

        result[0] = std::byte{ 0x4D };
        result[1] = std::byte{ 0x4C };
        result[2] = std::byte{ 0x01 };

		result[3] = static_cast<std::byte>(header.type);

        result[4] = static_cast<std::byte>((header.sequence >> 24) & 0xFF );
        result[5] = static_cast<std::byte>((header.sequence >> 16) & 0xFF );
        result[6] = static_cast<std::byte>((header.sequence >> 8) & 0xFF );
        result[7] = static_cast<std::byte>(header.sequence & 0xFF );

        result[8] = static_cast<std::byte>((header.payloadLength >> 8) & 0xFF);
        result[9] = static_cast<std::byte>(header.payloadLength & 0xFF );

		return result;
    }
}