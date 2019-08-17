#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <string>

typedef union InetAddress {
    uint8_t byte[4];
    in_addr_t inet;

    const static InetAddress UNKNOWN;

    InetAddress();
    InetAddress(const InetAddress &o);
    InetAddress(InetAddress &&o);
    InetAddress(const in_addr_t address);
    InetAddress(const uint8_t *address);

    InetAddress &operator=(const InetAddress &o);
    InetAddress &operator=(InetAddress &&o);

    operator in_addr_t();
    bool operator!=(const InetAddress &o) const;
    bool operator==(const InetAddress &o) const;

    bool operator>(const InetAddress &o) const;
    bool operator<(const InetAddress &o) const;
    bool operator>=(const InetAddress &o) const;
    bool operator<=(const InetAddress &o) const;

    std::string to_str() const;
    friend std::ostream &operator<<(std::ostream &os, const InetAddress &inaddr);
} InetAddress;

#endif // INETADDRESS_H
