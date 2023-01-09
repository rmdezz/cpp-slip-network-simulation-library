#pragma once
#include "List.h"
#include "PacketList.h"
#include "Module.h"

class ThrottleList : public List
{
	public:
		ThrottleList(PacketList& packetList, Module* pModule);
		virtual ~ThrottleList();
		void flushList();
		bool throttlePackets();
		bool shouldStartThrottling();
		void setStartTime();
		bool shouldThrottlePacket(Node*& node);
		bool shouldStopThrottling(DWORD& tick);
		void stopThrottling();
		bool shouldEndThrottling(DWORD& currentTime, DWORD& startTick, float throttleFrame);
		void dropPackets();
		bool shouldKeepThrottling(Node* pac);
		void startThrottlingPeriod(DWORD& currentTime);
		void endThrottlingPeriod(DWORD& currentTime);
		void manageThrottlingPeriod();
		void throttlePacket(Node*& pac);
		void printInfo();
		void printDroppedPacketsInfo();
		void printThrottleCapacityInfo();
		void deleteAllPackets();
		void resetStartTime();

	private:
		Module* pModule;
		PacketList& packetList;
		DWORD startTime;
};