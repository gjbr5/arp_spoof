#include "packetctrl.h"
#include "packetheader.h"
#include <algorithm>
#include <cstring>
#include <pcap.h>

std::string PacketCtrl::dev = "";
pcap_t *PacketCtrl::handle = nullptr;
char PacketCtrl::errbuf[PCAP_ERRBUF_SIZE] = {0};
std::map<void *, std::function<void(const uint8_t *)>> PacketCtrl::callbacks;
std::thread PacketCtrl::recv_thrd;
std::mutex PacketCtrl::mutex;
bool PacketCtrl::loop = true;

void PacketCtrl::init(std::string dev)
{
    PacketCtrl::dev = dev;
    recv_thrd = std::thread(PacketCtrl::recv_packet);
}

std::string PacketCtrl::get_device()
{
    return dev;
}

void PacketCtrl::insert_callback(void *which, std::function<void(const uint8_t *)> func)
{
    mutex.lock();
    callbacks.insert(std::pair<void *, std::function<void(const uint8_t *)>>(which, func));
    mutex.unlock();
}

void PacketCtrl::erase_callback(void *which)
{
    mutex.lock();
    callbacks.erase(callbacks.find(which));
    mutex.unlock();
}

void PacketCtrl::send_arp_request(InetAddress target)
{
    HostInfo my_info = HostInfo::my_info();
    ARPPacket packet;
    packet.eth.dmac = HWAddress::BROADCAST;
    packet.eth.smac = my_info.hwaddr;
    packet.eth.type = EthernetHeader::ARP_TYPE;
    packet.arp.htype = ARPHeader::HTYPE_ETH;
    packet.arp.ptype = ARPHeader::PTYPE_IP;
    packet.arp.hlen = ARPHeader::HLEN_ETH;
    packet.arp.plen = ARPHeader::PLEN_IP;
    packet.arp.oper = ARPHeader::OPER_REQ;
    packet.arp.sm = my_info.hwaddr;
    packet.arp.si = my_info.inaddr;
    packet.arp.tm = HWAddress::UNKNOWN;
    packet.arp.ti = target;

    for (int i = 0; i < 3; i++)
        pcap_sendpacket(handle, reinterpret_cast<const u_char *>(&packet), sizeof(packet));
}

void PacketCtrl::send_arp_reply(HWAddress sm, InetAddress si, HWAddress tm, InetAddress ti)
{
    ARPPacket packet;
    packet.eth.dmac = tm;
    packet.eth.smac = sm;
    packet.eth.type = EthernetHeader::ARP_TYPE;
    packet.arp.htype = ARPHeader::HTYPE_ETH;
    packet.arp.ptype = ARPHeader::PTYPE_IP;
    packet.arp.hlen = ARPHeader::HLEN_ETH;
    packet.arp.plen = ARPHeader::PLEN_IP;
    packet.arp.oper = ARPHeader::OPER_REP;
    packet.arp.sm = sm;
    packet.arp.si = si;
    packet.arp.tm = tm;
    packet.arp.ti = ti;

    pcap_sendpacket(handle, reinterpret_cast<const u_char *>(&packet), sizeof(packet));
}

void PacketCtrl::relay_ip_packet(const IPPacket *packet, HostInfo target)
{
    size_t length = packet->ip.total_length + sizeof(EthernetHeader);
    uint8_t *relay = new uint8_t[length];
    memcpy(relay, packet, length);

    EthernetHeader *eth = reinterpret_cast<EthernetHeader *>(relay);
    eth->smac = HostInfo::my_info().hwaddr;
    eth->dmac = target.hwaddr;

    pcap_sendpacket(handle, relay, int(length));

    delete[] relay;
}

void PacketCtrl::recv_packet()
{
    handle = pcap_open_live(dev.c_str(), BUFSIZ, 1, 1000, errbuf);
    while (loop) {
        struct pcap_pkthdr *header;
        const uint8_t *packet;
        int res = pcap_next_ex(handle, &header, &packet);
        if (res == 0)
            continue;
        if (res < 0)
            return;
        mutex.lock();
        for (std::pair<void *, std::function<void(const uint8_t *)>> pair : callbacks) {
            pair.second(packet);
        }
        mutex.unlock();
    }

    pcap_close(handle);
    exit(0);
}

void PacketCtrl::terminate(int)
{
    loop = false;
}
