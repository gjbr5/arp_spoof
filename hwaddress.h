#ifndef HWADDRESS_H
#define HWADDRESS_H

#include <cstdint>
#include <iostream>

typedef class HWAddress
{
private:
    uint8_t addr[6];

public:
    const static class HWAddress BROADCAST;
    const static class HWAddress UNKNOWN;

    HWAddress();
    HWAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f);
    HWAddress(const char addr[6]);

    HWAddress(const HWAddress &o);
    HWAddress(HWAddress &&o);
    HWAddress &operator=(const HWAddress &o);
    HWAddress &operator=(HWAddress &&o);

    bool operator!=(const HWAddress &o) const;
    bool operator==(const HWAddress &o) const;

    bool operator>(const HWAddress &o) const;
    bool operator<(const HWAddress &o) const;
    bool operator>=(const HWAddress &o) const;
    bool operator<=(const HWAddress &o) const;

    uint8_t &operator[](int idx);
    std::string to_str() const;

    friend std::ostream &operator<<(std::ostream &os, const HWAddress &hwaddr);
} HWAddress;

#endif // HWADDRESS_H
