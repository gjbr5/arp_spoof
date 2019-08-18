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

    static bool loop;

    static std::list<std::function<bool(time_t)>> periodic_callbacks;
    static std::mutex period_mutex;
    std::thread period_thrd;

    static std::list<std::function<bool(const uint8_t *)>> recv_callbacks;
    static std::mutex recv_mutex;
    std::thread recv_thrd;

    static void recv_packet();
    static void period_send();
    static PacketCtrl *instance;
    PacketCtrl();
    ~PacketCtrl();

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
