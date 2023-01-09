#pragma once
#include "List.h"
#include "PacketList.h"
#include "Module.h"

class ShuffleList : public List
{
	public:
		ShuffleList(PacketList& packetList, Module* pModule);
		virtual ~ShuffleList();

	private:
		PacketList& packetList;
		Module* pModule;
};