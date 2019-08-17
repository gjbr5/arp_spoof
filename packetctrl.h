#ifndef PACKETCTRL_H
#define PACKETCTRL_H

#include "hostinfo.h"
#include "inetaddress.h"
#include "packetheader.h"

#include <functional>
#include <list>
#include <mutex>
#include <pcap.h>
#include <thread>
#include <queue>

class Session;

class PacketCtrl
{
private:
    static std::string dev;
    static pcap_t *handle;
    static char errbuf[PCAP_ERRBUF_SIZE];

    static std::list<std::function<bool(time_t)>> periodic_callbacks;
    static std::list<std::function<bool(const uint8_t *)>> recv_callbacks;
    static std::mutex periodic_mutex;
    static std::thread recv_thrd;
    static std::mutex recv_mutex;

    static bool loop;

    static void recv_packet();

public:
    static void init(std::string dev);
    static std::string get_device();

    static void insert_callback(std::function<bool(time_t)> func);
    static void insert_callback(std::function<bool(const uint8_t *)> func);

    static void send_arp_request(InetAddress target);
    static void send_arp_reply(HWAddress sm, InetAddress si, HWAddress tm, InetAddress ti);

    static void relay_ip_packet(const IPPacket *packet, HostInfo target);

    static void terminate();
};

#endif // PACKETCTRL_H
