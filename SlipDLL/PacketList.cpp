#include "PacketList.h"
#include <iostream>
using namespace std;
#include <cstring>
#include "GlobalVar.h"
#include "messagePipe.h"

// status for sending packets
#define PACKET_SENT 1
#define PACKET_FAILED 0


PacketList::PacketList()
{
	
}

PacketList::~PacketList()
{
    //sendMessage("Packet list destroyed.");
    //cout << "Packet list destroyed." << endl;
}

void PacketList::insertPacket(Packet& packet)
{
    Node* node = new Node;
    node->packet = packet;
    addNodeBeforeTail(node);
}

int PacketList::sendPackets()
{
    /* Send packets from tail to head and remove sent ones */
    int sendCount = 0;
    while (!isListEmpty())
    {
        if (GlobalVar::GetInstance()->divertHandle == INVALID_HANDLE_VALUE)
        {
            // handle was closed
            return sendCount;
        }

        Node* node = popNode(getTail()->prev);
        if (node == getHead()) break;
        sendCount += sendPacket(node);
        sendMessage("Freeing up memory previously allocated for packet's node.");
        //GlobalVar::GetInstance()->printMessage("Freeing up memory previously allocated for packet nodes.");
        delete node;
    }
    return sendCount;
}

int PacketList::sendPacket(Node* node)
{
    UINT sendLength = 0;
    HANDLE& divertHandle = GlobalVar::GetInstance()->divertHandle;
    Packet& packet = node->packet;

    if (divertHandle == INVALID_HANDLE_VALUE) return PACKET_FAILED;

    if (sendThroughDivert(divertHandle, node, sendLength) && packetFullySent(packet.getPacketLen(), sendLength))
    {
        sendMessage("Packet sent to the network stack.");
        return PACKET_SENT;
        //GlobalVar::GetInstance()->printMessage("Packet sent to the network stack.");
        //return updateSendState(SEND_STATUS_SEND, 1);
    }

    /*
        As stated in windivert help, reinjecting
        inbound ICMP packets could fail on occasion;
        this can be circumvented by resending them
        as outbound.
    */

    else if (is_ICMP_packet(packet.getPacket(), packet.getPacketLen()) && isInbound(packet.getPacketAddress()))
    {
        return sendIcmpPacket(node);
    }

    return PACKET_FAILED; //updateSendState(SEND_STATUS_FAIL, 0);
}

bool PacketList::is_ICMP_packet(char* packet, UINT packetLen)
{
    UINT8 protocol;
    WinDivertHelperParsePacket(packet, packetLen, NULL, NULL, &protocol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return protocol == IPPROTO_ICMP;
}

bool PacketList::isInbound(WINDIVERT_ADDRESS* packetAddress)
{
    return !packetAddress->Outbound;
}

bool PacketList::packetFullySent(UINT packetLen, UINT len)
{
    return len == packetLen;
}

bool PacketList::sendThroughDivert(HANDLE& divertHandle, Node*& node, UINT& len)
{
    // TRUE if a packet was successfully injected, or FALSE if an error occurred
    // if divert handle is invalid, then it returns false

    if (divertHandle == INVALID_HANDLE_VALUE) return false;
    return WinDivertSend(divertHandle, node->packet.getPacket(), node->packet.getPacketLen(), &len, node->packet.getPacketAddress());
}

int PacketList::sendIcmpPacket(Node*& node)
{
    flipDirection(node);
    UINT sendLen = 0;

    HANDLE& divertHandle = GlobalVar::GetInstance()->divertHandle;
    if (sendThroughDivert(divertHandle, node, sendLen) && packetFullySent(node->packet.getPacketLen(), sendLen))
    {
        return PACKET_SENT;//updateSendState(SEND_STATUS_SEND, 1);
    }
    else
    {
        return PACKET_FAILED;// updateSendState(SEND_STATUS_FAIL, 0);
    }
}

void PacketList::flipDirection(Node*& node)
{
    node->packet.getPacketAddress()->Outbound = true;
    swapIpAddresses(node->packet.getPacket(), node->packet.getPacketLen());
}

void PacketList::swapIpAddresses(char* packet, UINT packetLen)
{
    PWINDIVERT_IPHDR ipHeader;
    PWINDIVERT_IPV6HDR ipv6Header;
    WinDivertHelperParsePacket(packet, packetLen, &ipHeader, &ipv6Header, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    if (ipHeader)
    {
        UINT32 tmp = ipHeader->SrcAddr;
        ipHeader->SrcAddr = ipHeader->DstAddr;
        ipHeader->DstAddr = tmp;
    }
    else
    {
        UINT32 tmp_arr[4];
        memcpy(tmp_arr, ipv6Header->SrcAddr, sizeof(tmp_arr));
        memcpy(ipv6Header->SrcAddr, ipv6Header->DstAddr, sizeof(tmp_arr));
        memcpy(ipv6Header->DstAddr, tmp_arr, sizeof(tmp_arr));
    }
}