#include "LagModule.h"
#include "messagePipe.h"

LagModule::LagModule(PacketList& packetList) : Module(packetList), lagList(packetList, this)
{
	setDescription("Temporarily delay packets in order to simulate network lag");
	setShortName("Lag");
	//setTime(500);
}

LagModule::~LagModule()
{
	
}

void LagModule::startUp()
{
	Module::startUp();
}

void LagModule::closeDown()
{
	lagList.flushList();
	Module::closeDown();
}

bool LagModule::process()
{
	return lagList.processPackets();
}


