#pragma once
#include "Module.h"

class DropModule : public Module
{
	public:
		DropModule(PacketList& packetList);
		~DropModule();
		void startUp();
		void closeDown();
		bool process() override;
		bool dropRandomPackets();
		bool shouldContinueProcessing(Node*& current);
		void processCurrentNode(Node*& current, Node*& headC, int& dropped);
		bool shouldDropPacket(Node*& node);
		void printInfo(Node*& current);

	private:
		
};

