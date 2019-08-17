#include "session.h"
#include "hostinfo.h"
#include "packetctrl.h"
#include <cstring>
#include <sstream>
#include <thread>

Session::Session(HostInfo sender, HostInfo target)
    : sender(sender)
    , target(target)
{}

Session::~Session()
{
    if (isOpened)
        close();
}

void Session::open()
{
    if (isOpened)
        return;
    isOpened = true;
    std::cout << "Session Opened: " << to_str() << std::endl;

    PacketCtrl::insert_callback([&](const uint8_t *packet) -> bool {
        if (!isOpened) {
            return true;
        }

        if (packet == nullptr)
            return false;

        const EthernetHeader *eth_hdr = reinterpret_cast<const EthernetHeader *>(packet);
        if (eth_hdr->smac != sender.hwaddr)
            return false;

        if (eth_hdr->type == EthernetHeader::ARP_TYPE) {
            const ARPPacket *arp_pkt = reinterpret_cast<const ARPPacket *>(packet);
            if (arp_pkt->arp.ti == target.inaddr) {
                PacketCtrl::send_arp_reply(HostInfo::my_info().hwaddr,
                                           target.inaddr,
                                           sender.hwaddr,
                                           sender.inaddr);
            }

        } else if (eth_hdr->type == EthernetHeader::IP_TYPE) {
            const IPPacket *ip_pkt = reinterpret_cast<const IPPacket *>(packet);
            if (ip_pkt->ip.dst_ip != HostInfo::my_info().inaddr)
                PacketCtrl::relay_ip_packet(ip_pkt, target);
        }
        return false;
    });

    PacketCtrl::insert_callback([&](time_t now) -> bool {
        if (!isOpened)
            return true;
        static time_t last = 0;
        if (now - last > 5) {
            for (int i = 0; i < 3; i++)
                PacketCtrl::send_arp_reply(HostInfo::my_info().hwaddr,
                                           target.inaddr,
                                           sender.hwaddr,
                                           sender.inaddr);
            last = now;
        }
        return false;
    });
    for (int i = 0; i < 3; i++)
        PacketCtrl::send_arp_reply(HostInfo::my_info().hwaddr,
                                   target.inaddr,
                                   sender.hwaddr,
                                   sender.inaddr);
}

void Session::close()
{
    if (!isOpened)
        return;
    isOpened = false;
    std::cout << "Session closed: " << to_str() << std::endl;
    for (int i = 0; i < 3; i++)
        PacketCtrl::send_arp_reply(target.hwaddr, target.inaddr, sender.hwaddr, sender.inaddr);
}

std::string Session::to_str()
{
    std::stringstream ss;
    ss << "Session: (" << sender.inaddr.to_str() << " to " << target.inaddr.to_str() << ")";
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, Session &session)
{
    os << session.to_str();
    return os;
}
