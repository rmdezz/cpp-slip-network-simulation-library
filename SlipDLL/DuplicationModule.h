#pragma once
#include <queue>
#include "Module.h"
#include <mutex>

class DuplicationModule : public Module
{
	public:
		DuplicationModule(PacketList& packetList);
		~DuplicationModule();
		void startUp();
		void closeDown() override;
		bool process() override;
		bool duplicatePackets();
		bool shouldKeepDuplicating(Node*& current);
		void printInfo();
		bool shouldDuplicatePacket(Node*& current);
		void duplicatePacket(Node*& current);
		void createCopies(Node*& current);
		void insertCopies(Node*& current);
		void duplicateCurrentPacket(Node*& current);
		void modifySequenceNumber(Node*& node, UINT32& prevSegNum, UINT& prevPayloadLen);
		void insertPacketsFromQueue(Node*& current, UINT32& prevSeqNum, UINT& prevPayloadLen);
		void updateSequenceNumber(UINT32& prevSegNum, UINT& prevPayloadLen, PWINDIVERT_TCPHDR& tcpHeader, UINT& payloadLen);

	private:
		std::mutex queueMutex;
		std::queue<Node*> copies;

		/*
		*	An atomic int is a type of variable that is designed to be
		*	accessed and modified by multiple threads concurrently, in
		*	a way that is safe and avoids race conditions.
		* 
		*	For example, if two threads try to increment the value of an atomic
		*	int simultaneously, the operations will be performed in a way that
		*	ensures that the resulting value is always correct, without any risk
		*	of one of the operations being lost or overwritten.
		*/
		std::atomic<int> threadsFinished;
};