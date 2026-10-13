#include <iostream>
#include "protocol/ProtocolHeader.h"

int main()
{
    protocol::Header header{ 2, 0x12345678, 3 };

    auto encoded = protocol::EncodeHeader(header);

    std::cout << "Encoded header size: "
        << encoded.size() << '\n';

	auto decoded = protocol::DecodeHeader(encoded);

    if (decoded)
    {
        std::cout << "Decoded header:\n";

        std::cout << "Type: "
            << static_cast<int>(decoded->type)
            << '\n';

        std::cout << "Sequence: "
            << decoded->sequence << '\n';

        std::cout << "Payload length: "
            << decoded->payloadLength << '\n';
    }
    else
    {
        std::cout << "Failed to decode header.\n";
    }

    return 0;
}