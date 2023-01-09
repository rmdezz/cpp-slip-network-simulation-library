#pragma once
#include "List.h"

class PacketList : public List
{
	public:
		PacketList();
		virtual ~PacketList();

		void insertPacket(Packet& packet);
		int sendPackets();
		int sendPacket(Node* node);
		bool sendThroughDivert(HANDLE& divertHandle, Node*& node, UINT& len);
		bool packetFullySent(UINT packetLen, UINT len);
		bool is_ICMP_packet(char* packet, UINT packetLen);
		bool isInbound(WINDIVERT_ADDRESS* packetAddress);
		int sendIcmpPacket(Node*& node);
		void flipDirection(Node*& node);
		void swapIpAddresses(char* packet, UINT packetLen);

	private:
		
};

