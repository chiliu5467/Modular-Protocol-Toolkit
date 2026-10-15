#pragma once

#include "protocol/HeaderPolicy.h"
#include "protocol/ProtocolHeader.h"

namespace protocol
{
    class BasicHeaderPolicy final : public HeaderPolicy
    {
    public:
        bool Accept(const Header& header) const override;
    };
}