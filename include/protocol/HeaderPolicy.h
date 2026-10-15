#pragma once

namespace protocol
{
    struct Header;

    class HeaderPolicy
    {
    public:
        virtual ~HeaderPolicy() = default;

        virtual bool Accept(const Header& header) const = 0;
    };
}