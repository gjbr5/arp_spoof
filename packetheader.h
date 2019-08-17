#ifndef PACKETHEADER_H
#define PACKETHEADER_H
#include "hwaddress.h"
#include "inetaddress.h"
#include <arpa/inet.h>
#include <cstdint>

#pragma pack(1)

typedef struct EthernetHeader
{
    HWAddress dmac;
    HWAddress smac;
    uint16_t type;

    const static uint16_t IP_TYPE;
    const static uint16_t ARP_TYPE;

} EthernetHeader;

typedef struct ARPHeader
{
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    HWAddress sm;
    InetAddress si;
    HWAddress tm;
    InetAddress ti;

    const static uint16_t HTYPE_ETH;
    const static uint16_t PTYPE_IP;
    const static uint8_t HLEN_ETH;
    const static uint8_t PLEN_IP;
    const static uint16_t OPER_REQ;
    const static uint16_t OPER_REP;
} ARPHeader;

typedef struct IPHeader
{
    struct
    {
        uint8_t version : 4;
        uint8_t header_length : 4;
    } vhl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    struct
    {
        bool o : 1;
        bool df : 1;
        bool mf : 1;
        uint16_t offset : 13;
    } fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    InetAddress src_ip;
    InetAddress dst_ip;
} IPHeader;

typedef struct ARPPacket
{
    EthernetHeader eth;
    ARPHeader arp;
} ARPPacket;

typedef struct IPPacket
{
    EthernetHeader eth;
    IPHeader ip;
    std::string to_str() const;
    friend std::ostream &operator<<(std::ostream &os, const IPPacket &ippacket);
} IPPacket;

#pragma pack()

#endif // PACKETHEADER_H
