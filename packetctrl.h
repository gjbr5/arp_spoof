#ifndef PACKETCTRL_H
#define PACKETCTRL_H

#include "hostinfo.h"
#include "inetaddress.h"
#include "packetheader.h"

#include <functional>
#include <map>
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

    static std::map<void *, std::function<void(const uint8_t *)>> callbacks;

    static std::thread recv_thrd;
    static std::mutex mutex;

    static bool loop;

    static void recv_packet();

public:
    static void init(std::string dev);
    static std::string get_device();

    static void insert_callback(void *which, std::function<void(const uint8_t *)> func);
    static void erase_callback(void *which);

    static void send_arp_request(InetAddress target);
    static void send_arp_reply(HWAddress sm, InetAddress si, HWAddress tm, InetAddress ti);

    static void relay_ip_packet(const IPPacket *packet, HostInfo target);

    static void terminate(int);
};

#endif // PACKETCTRL_H
