#include "protocol/BasicHeaderPolicy.h"

namespace protocol
{
    bool BasicHeaderPolicy::Accept(const Header& header) const
    {
        if (header.type == 1 || header.type == 2)
        {
            if (header.payloadLength <= 1024)
            {
                return true;
            }
        }

        return false;
    }
}