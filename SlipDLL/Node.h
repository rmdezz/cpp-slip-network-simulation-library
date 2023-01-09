#pragma once

#include "windivert.h"
#include "Packet.h"
class Node
{
	public:
		Node();
		virtual ~Node();
		friend class PacketList;
		friend class LagList;
		friend class ThrottleList;
		friend class Module;
		friend class DropModule;
		friend class List;
		friend class DuplicationModule;
		friend class ShuffleModule;
		friend class EncryptModule;
		friend class TrafficShaperModule;

	private:
		Packet packet;
		Node* prev;
		Node* next;
};



