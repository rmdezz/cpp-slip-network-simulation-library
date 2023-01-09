#pragma once
#include "List.h"
#include "PacketList.h"
#include "Module.h"

class LagList : public List
{
	public:

		/*
		 * Here, the member variable packetList is initialized with the value of the
		 * packetList argument passed to the constructor. This ensures that the
		 * packetList member variable in the LagList class refers to the same object
		 * as the packetList argument passed to the constructor.
		 */
		LagList(PacketList& packetList, Module* pModule);
		virtual ~LagList();
		bool processPackets();
		DWORD addPacketsToLagList();
		bool shouldKeepAddingPackets(Node*& node);
		bool isBufferFull();
		bool isPacketListEmpty(Node*& node);
		bool isPacketDirectionValid(WINDIVERT_ADDRESS* packetAddress);
		void printInfo();
		void addPacketToLagList(Node* current);

		void releaseReadyPackets(DWORD currentTime);
		bool shouldSendPacket(Node*& node, DWORD currentTime);
		bool isDelayTimeExceeded(Node*& node, float lagTime, DWORD currentTime);
	
		void flushBuffer();
		bool shouldFlushPacket(int& flushCount);
		void flushList();

		bool shouldAddPacket(Node* node);

	private:
		// Store a pointer to a Module object
		Module* pModule;
		// Declare a reference to the PacketList object as a member variable of LagList
		PacketList& packetList;
};

