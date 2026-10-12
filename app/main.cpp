#include <iostream>
#include "protocol/ProtocolHeader.h"

int main()
{
    protocol::Header header{ 2, 0x12345678, 3 };

    auto encoded = protocol::EncodeHeader(header);

    std::cout << "Encoded header size: "
        << encoded.size() << '\n';

    return 0;
}