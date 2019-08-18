#include "packetctrl.h"
#include "packetheader.h"
#include "session.h"
#include <algorithm>
#include <cstring>
#include <pcap.h>
#include <set>
#include <unistd.h>

std::string PacketCtrl::dev = "";
pcap_t *PacketCtrl::handle = nullptr;
char PacketCtrl::errbuf[PCAP_ERRBUF_SIZE] = {0};

std::mutex PacketCtrl::period_mutex;
std::list<std::function<bool(time_t)>> PacketCtrl::periodic_callbacks;
std::mutex PacketCtrl::recv_mutex;
std::list<std::function<bool(const uint8_t *)>> PacketCtrl::recv_callbacks;
bool PacketCtrl::loop = true;
PacketCtrl *PacketCtrl::instance = nullptr;

PacketCtrl::PacketCtrl()
    : period_thrd(PacketCtrl::period_send)
    , recv_thrd(PacketCtrl::recv_packet)
{}

PacketCtrl::~PacketCtrl()
{
    period_thrd.detach();
    recv_thrd.detach();
}

void PacketCtrl::init(std::string dev)
{
    PacketCtrl::dev = dev;
    handle = pcap_open_live(dev.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (instance == nullptr)
        instance = new PacketCtrl();
}

std::string PacketCtrl::get_device()
{
    return dev;
}

void PacketCtrl::insert_callback(std::function<bool(time_t)> func)
{
    period_mutex.lock();
    periodic_callbacks.push_back(func);
    period_mutex.unlock();
}

void PacketCtrl::insert_callback(std::function<bool(const uint8_t *)> func)
{
    recv_mutex.lock();
    recv_callbacks.push_back(func);
    recv_mutex.unlock();
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

    std::cout << "Sending ARP Packet: (" << si.to_str() << " = " << sm.to_str() << ") to "
              << ti.to_str() << "...\n";

    pcap_sendpacket(handle, reinterpret_cast<const u_char *>(&packet), sizeof(packet));
}

void PacketCtrl::relay_ip_packet(const IPPacket *packet, HostInfo target)
{
    size_t length = ntohs(packet->ip.total_length) + sizeof(EthernetHeader);
    uint8_t *relay = new uint8_t[length];
    memcpy(relay, packet, length);

    EthernetHeader *eth = reinterpret_cast<EthernetHeader *>(relay);
    eth->smac = HostInfo::my_info().hwaddr;
    eth->dmac = target.hwaddr;

    std::cout << "IP Packet Relayed: " << packet->ip.src_ip.to_str() << " to "
              << packet->ip.dst_ip.to_str() << "\n";

    pcap_sendpacket(handle, relay, int(length));

    delete[] relay;
}

void PacketCtrl::period_send()
{
    while (loop) {
        period_mutex.lock();
        time_t now = time(nullptr);
        for (auto it = periodic_callbacks.begin(); it != periodic_callbacks.end(); it++) {
            if ((*it)(now)) {
                periodic_callbacks.erase(it);
                it = periodic_callbacks.begin();
            }
        }
        period_mutex.unlock();
    }
    period_mutex.lock();
    periodic_callbacks.clear();
    period_mutex.unlock();
}

void PacketCtrl::recv_packet()
{
    while (loop) {
        struct pcap_pkthdr *header;
        const uint8_t *packet;

        int res = pcap_next_ex(handle, &header, &packet);
        if (res == 0)
            continue;
        if (res < 0)
            return;
        recv_mutex.lock();
        for (auto it = recv_callbacks.begin(); it != recv_callbacks.end(); it++) {
            if ((*it)(packet)) {
                recv_callbacks.erase(it);
                it = recv_callbacks.begin();
            }
        }
        recv_mutex.unlock();
    }
    recv_mutex.lock();
    recv_callbacks.clear();
    recv_mutex.unlock();
    pcap_close(handle);
}

void PacketCtrl::terminate()
{
    loop = false;

    delete instance;
    instance = nullptr;
}
