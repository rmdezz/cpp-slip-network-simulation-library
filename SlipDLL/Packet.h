#pragma once
#include "windivert.h"

class Packet
{
	public:
		Packet();
		virtual ~Packet();
		Packet(char* packetBuf, UINT packetLen, WINDIVERT_ADDRESS* packetAddr);
		Packet(const Packet& orig);
		void operator = (const Packet& orig);
		char* getPacket() const;
		void setPacket(char* packet, unsigned int packetLen);
		unsigned int getPacketLen();
		void setPacketLen(unsigned int packetLen);
		void setPacketAddress(const WINDIVERT_ADDRESS* packetAddress);
		void setTimestamp(unsigned long long timestamp);
		void dumpPacket(char* buffer, UINT len, PWINDIVERT_ADDRESS paddr);
		WINDIVERT_ADDRESS* getPacketAddress();
		DWORD getTimestamp();
		void print() const;

	private:
		char* packet;
		unsigned int packetLen;
		WINDIVERT_ADDRESS packet_address; // Hold packet metadata
		unsigned long long timestamp;
};

