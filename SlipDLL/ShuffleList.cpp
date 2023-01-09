#include "ShuffleList.h"

ShuffleList::ShuffleList(PacketList& packetList, Module* pModule) : packetList(packetList), pModule(pModule)
{
	setName("Shuffle");
}

ShuffleList::~ShuffleList()
{

}