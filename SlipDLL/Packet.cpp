#include "Packet.h"
#include <cstring>
#pragma comment(lib, "WinDivert.lib")
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>

Packet::Packet()
{
	packet = nullptr;
	packetLen = 0;
	timestamp = 0;
}

Packet::~Packet()
{
	if (packet != nullptr) delete[] this->packet;
}

Packet::Packet(char* packetBuf, UINT packetLen, WINDIVERT_ADDRESS* packetAddr)
{
	timestamp = 0;
	setPacket(packetBuf, packetLen);
	setPacketLen(packetLen);
	setPacketAddress(packetAddr);
}

char* Packet::getPacket() const
{
	return this->packet;
}

void Packet::setPacket(char* packet, unsigned int packetLen)
{
	if (this->packet != nullptr) delete[] this->packet;
	this->packet = new char[packetLen];
	void* dest = this->packet;
	void* src = packet;
	memcpy(dest, src, packetLen);
}

Packet::Packet(const Packet& orig)
{
	packet = nullptr;
	*this = orig;
}

void Packet::operator = (const Packet& orig)
{
	char* packet = orig.getPacket();
	
	//std::cout << "Original" << std::endl;
	//orig.print();

	setPacket(packet, orig.packetLen);
	setPacketLen(orig.packetLen);
	setPacketAddress(&orig.packet_address);
	setTimestamp(orig.timestamp);

	//std::cout << "Copy" << std::endl;
	//print();
}

void Packet::print() const
{
	std::cout << "Packet: " << packet << std::endl;
	std::cout << "Packet len: " << packetLen << std::endl;
	std::cout << "Timestamp: " << timestamp << std::endl;
}

unsigned int Packet::getPacketLen()
{
	return this->packetLen;
}

void Packet::setPacketLen(unsigned int packetLen)
{
	this->packetLen = packetLen;
}

void Packet::setPacketAddress(const WINDIVERT_ADDRESS* packetAddress)
{
	void* dest = &(this->packet_address);
	const void* src = packetAddress;
	memcpy(dest, src, sizeof(WINDIVERT_ADDRESS));
}

void Packet::setTimestamp(unsigned long long timestamp)
{
	this->timestamp = timestamp;
}

DWORD Packet::getTimestamp()
{
	return this->timestamp;
}


void Packet::dumpPacket(char* buffer, UINT len, PWINDIVERT_ADDRESS paddr)
{
	PWINDIVERT_IPHDR dbg_ip_header;
	PWINDIVERT_IPV6HDR dbg_ipv6_header;
	PWINDIVERT_TCPHDR dbg_tcp_header;
	PWINDIVERT_UDPHDR dbg_udp_header;
	PWINDIVERT_ICMPHDR dbg_icmp_header;
	PWINDIVERT_ICMPV6HDR dbg_icmpv6_header;
	UINT payload_len;

	WinDivertHelperParsePacket(buffer, len, &dbg_ip_header, &dbg_ipv6_header, nullptr,
		&dbg_icmp_header, &dbg_icmpv6_header, &dbg_tcp_header, &dbg_udp_header,
		nullptr, &payload_len, nullptr, nullptr);

	char protocol[20];
	UINT16 srcPort = 0, dstPort = 0;

	if (dbg_tcp_header != nullptr)
	{
		strcpy_s(protocol, sizeof(protocol), "TCP");
		srcPort = ntohs(dbg_tcp_header->SrcPort);
		dstPort = ntohs(dbg_tcp_header->DstPort);
	}
	else if (dbg_udp_header != nullptr)
	{
		
	}
}

WINDIVERT_ADDRESS* Packet::getPacketAddress()
{
	return &(this->packet_address);
}

