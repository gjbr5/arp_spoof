#include "inetaddress.h"
#include <cstring>
#include <sstream>

const InetAddress InetAddress::UNKNOWN = 0U;

InetAddress::InetAddress()
    : inet(0)
{}

InetAddress::InetAddress(const InetAddress &o)
    : inet(o.inet)
{}

InetAddress::InetAddress(InetAddress &&o)
    : inet(o.inet)
{}

InetAddress::InetAddress(in_addr_t address)
    : inet(address)
{}

InetAddress::InetAddress(const uint8_t *address)
{
    for (int i = 0; i < 4; i++) {
        byte[i] = address[i];
    }
}

InetAddress &InetAddress::operator=(const InetAddress &o)
{
    inet = o.inet;
    return *this;
}

InetAddress &InetAddress::operator=(InetAddress &&o)
{
    inet = o.inet;
    return *this;
}

InetAddress::operator in_addr_t()
{
    return inet;
}

bool InetAddress::operator!=(const InetAddress &o) const
{
    return inet != o.inet;
}

bool InetAddress::operator==(const InetAddress &o) const
{
    return !(*this != o);
}

bool InetAddress::operator>(const InetAddress &o) const
{
    return memcmp(byte, o.byte, 4) > 0;
}
bool InetAddress::operator<(const InetAddress &o) const
{
    return !(*this >= o);
}
bool InetAddress::operator>=(const InetAddress &o) const
{
    return *this > o || *this == o;
}
bool InetAddress::operator<=(const InetAddress &o) const
{
    return !(*this > o);
}

std::string InetAddress::to_str() const
{
    std::stringstream ss;
    for (int i = 0; i < 3; i++)
        ss << +byte[i] << ".";
    ss << +byte[3];
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const InetAddress &inaddr)
{
    os << inaddr.to_str();
    return os;
}
