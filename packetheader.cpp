#include "packetheader.h"
#include <sstream>

const uint16_t EthernetHeader::IP_TYPE = htons(0x0800);
const uint16_t EthernetHeader::ARP_TYPE = htons(0x0806);

const uint16_t ARPHeader::HTYPE_ETH = htons(0x0001);
const uint16_t ARPHeader::PTYPE_IP = htons(0x0800);
const uint8_t ARPHeader::HLEN_ETH = 6;
const uint8_t ARPHeader::PLEN_IP = 4;
const uint16_t ARPHeader::OPER_REQ = htons(0x0001);
const uint16_t ARPHeader::OPER_REP = htons(0x0002);

std::string IPPacket::to_str() const
{
    std::stringstream ss;
    ss << "IPPacket: " << ip.src_ip << " to " << ip.dst_ip;
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const IPPacket &ippacket)
{
    return os << ippacket.to_str();
}
