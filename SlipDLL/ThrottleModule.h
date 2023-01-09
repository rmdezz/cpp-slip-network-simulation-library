#pragma once

#include "ThrottleList.h"
#include "Module.h"

class ThrottleModule : public Module
{
	public:
		ThrottleModule(PacketList& packetList);
		~ThrottleModule();
		void startUp();
		void closeDown() override;
		bool process() override;

	private:
		ThrottleList throttleList;
};