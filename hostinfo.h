#ifndef HOSTINFO_H
#define HOSTINFO_H

#include "hwaddress.h"
#include "inetaddress.h"
#include <arpa/inet.h>
#include <string>

class HostInfo
{
private:
    static std::string dev;
    static HostInfo MY_INFO;

    HostInfo(HWAddress hwaddr, InetAddress inaddr);

public:
    HWAddress hwaddr;
    InetAddress inaddr;

    HostInfo(InetAddress inaddr);

    std::string to_str();
    static HostInfo my_info();
};

#endif // HOSTINFO_H
