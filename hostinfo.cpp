#include "hostinfo.h"
#include "packetctrl.h"
#include <cstdlib>
#include <cstring>
#include <net/if.h>
#include <sstream>
#include <sys/ioctl.h>
#include <unistd.h>

std::string HostInfo::dev = "";
HostInfo HostInfo::MY_INFO(HWAddress::UNKNOWN, InetAddress::UNKNOWN);

HostInfo::HostInfo(InetAddress inaddr)
    : inaddr(inaddr)
{
    hwaddr = HWAddress::UNKNOWN;
    PacketCtrl::insert_callback([&](const uint8_t *packet) -> bool {
        const ARPPacket *arp_pkt = reinterpret_cast<const ARPPacket *>(packet);
        if (arp_pkt->eth.type != EthernetHeader::ARP_TYPE)
            return false;
        if (arp_pkt->arp.si != inaddr)
            return false;
        hwaddr = arp_pkt->arp.sm;
        return true;
    });

    for (unsigned int i = 1; hwaddr == HWAddress::UNKNOWN; i++) {
        if (i > 5) {
            std::cerr << "Can't Get " << inaddr << "\'s HWAddress\n";
            exit(1);
        }
        std::cout << "Getting " << inaddr << "\'s HWAddress...(" << i << ")\n";
        PacketCtrl::send_arp_request(inaddr);
        sleep(i);
    }

    std::cout << to_str() << std::endl;
}

HostInfo::HostInfo(HWAddress hwaddr, InetAddress inaddr)
    : hwaddr(hwaddr)
    , inaddr(inaddr)
{
    if (hwaddr != HWAddress::UNKNOWN)
        std::cout << to_str() << std::endl;
}

std::string HostInfo::to_str()
{
    std::stringstream ss;
    ss << "HostInfo: (" << inaddr.to_str() << " = " << hwaddr.to_str() << ")";
    return ss.str();
}

HostInfo HostInfo::my_info()
{
    if (dev != "")
        return MY_INFO;
    if ((dev = PacketCtrl::get_device()) == "") {
        std::cerr << "Please Set Device!\n";
        exit(1);
    }

    HWAddress hw;
    {
        struct ifreq ifr;
        strncpy(ifr.ifr_name, dev.c_str(), sizeof(dev) - 1);

        int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock < 0 || ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
            exit(1);
        close(sock);
        hw = HWAddress(ifr.ifr_hwaddr.sa_data);
    }

    InetAddress in;
    {
        struct ifreq ifr;
        strncpy(ifr.ifr_name, dev.c_str(), sizeof(dev) - 1);
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0 || ioctl(sock, SIOCGIFADDR, &ifr) < 0)
            exit(1);
        close(sock);
        in = InetAddress(reinterpret_cast<const uint8_t *>(ifr.ifr_addr.sa_data + 2));
    }

    HostInfo::MY_INFO = HostInfo(hw, in);

    return MY_INFO;
}
