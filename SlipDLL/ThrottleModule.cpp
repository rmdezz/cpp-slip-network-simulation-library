#include "ThrottleModule.h"


ThrottleModule::ThrottleModule(PacketList& packetList) : Module(packetList), throttleList(packetList, this)
{
	setDescription("Throttles traffic by delaying packets for a specified time before sending or dropping them in a batch.");
	setShortName("Throttle");
}

ThrottleModule::~ThrottleModule()
{

}

void ThrottleModule::startUp()
{
	Module::startUp();
}

void ThrottleModule::closeDown()
{
	throttleList.flushList();
	Module::closeDown();
}

bool ThrottleModule::process()
{
	return throttleList.throttlePackets();
}