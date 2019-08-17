#include "hwaddress.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

const HWAddress HWAddress::BROADCAST = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const HWAddress HWAddress::UNKNOWN = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

HWAddress::HWAddress()
{
    memset(addr, 0, 6);
}

HWAddress::HWAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f)
{
    addr[0] = a;
    addr[1] = b;
    addr[2] = c;
    addr[3] = d;
    addr[4] = e;
    addr[5] = f;
}

HWAddress::HWAddress(const char addr[6])
{
    memcpy(this->addr, addr, 6);
}

HWAddress::HWAddress(const HWAddress &o)
{
    if (this == &o)
        return;
    memcpy(addr, o.addr, 6);
}

HWAddress::HWAddress(HWAddress &&o)
{
    memcpy(addr, o.addr, 6);
}

HWAddress &HWAddress::operator=(const HWAddress &o)
{
    if (this == &o)
        return *this;
    memcpy(addr, o.addr, 6);
    return *this;
}

HWAddress &HWAddress::operator=(HWAddress &&o)
{
    memcpy(addr, o.addr, 6);
    return *this;
}

bool HWAddress::operator!=(const HWAddress &o) const
{
    return memcmp(addr, o.addr, 6);
}

bool HWAddress::operator==(const HWAddress &o) const
{
    return !(*this != o);
}

bool HWAddress::operator>(const HWAddress &o) const
{
    return memcmp(addr, o.addr, 6) > 0;
}

bool HWAddress::operator<(const HWAddress &o) const
{
    return !(*this >= o);
}

bool HWAddress::operator>=(const HWAddress &o) const
{
    return *this > o || *this == o;
}

bool HWAddress::operator<=(const HWAddress &o) const
{
    return !(*this > o);
}

uint8_t &HWAddress::operator[](int idx)
{
    return addr[idx];
}

std::string HWAddress::to_str() const
{
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 5; i++)
        ss << std::setfill('0') << std::setw(2) << +addr[i] << ":";
    ss << std::setfill('0') << std::setw(2) << +addr[5];
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const HWAddress &hwaddr)
{
    std::cout << hwaddr.to_str();
    return os;
}
