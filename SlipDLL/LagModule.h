#pragma once
#include "Module.h"
#include "LagList.h"
class LagModule : public Module
{
	public:
		LagModule(PacketList& packetList);
		~LagModule();
		void startUp();
		void closeDown() override;
		bool process() override;

	private:
		LagList lagList;
};

