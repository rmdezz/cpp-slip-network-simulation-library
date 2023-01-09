#include "TrafficShaperModule.h"
#include "GlobalVar.h"
#include "windivert.h"
#include "Node.h"
#include "messagePipe.h"
#include <sstream>
#define WINDOW_SIZE 100000 // 100 KB

TrafficShaperModule::TrafficShaperModule(PacketList& packetList) : Module(packetList)
{
	setDescription("limits the speed of incoming and outgoing traffic on a network connection to a specific value.");
	setShortName("Traffic Shaper");
	currentSpeed = currentWindow = currentTime = lastTime = 0;
}

TrafficShaperModule::~TrafficShaperModule()
{

}

void TrafficShaperModule::startUp()
{
	Module::startUp();
}

void TrafficShaperModule::closeDown()
{
	Module::closeDown();
}

bool TrafficShaperModule::process()
{
	Node* current = packetList.getTail()->prev;

	while (shouldKeepProcessing(current))
	{
		if (shouldWork(current))
		{
			printInfo();

			// Get the current time.
			updateCurrentTime();

			// Update the current speed.
			updateCurrentSpeed(current);

			resetCurrentSpeedAndWindowIfNeeded();

			// If the current speed is greater than the speed limit, delay the packet.
			if (isOverSpeedLimit(currentSpeed))
			{
				delayPacket(current);
			}

			// If the current window + the packet length is greater
			// than the window size, discard the packet.
			if (isWindowOverSize(current))
			{
				discardPacket(current);
			}
			else
			{
				updateCurrentWindow(current);
			}

			// Update the time of the last packet.
			updateLastTime();
		}

		// Pass to the next packet
		current = current->prev;
			
	}

	return true;
}

void TrafficShaperModule::updateCurrentTime()
{
	currentTime = GetTickCount64();
}

void TrafficShaperModule::updateCurrentSpeed(Node*& current)
{
	currentSpeed += current->packet.getPacketLen();
}

void TrafficShaperModule::resetCurrentSpeedAndWindowIfNeeded()
{
	// If the current time is greater than the time of
	// the last packet + the window size, reset the current
	// speed and window:
	if (currentTime > lastTime + WINDOW_SIZE)
	{
		currentSpeed = 0;
		currentWindow = 0;
	}
}

bool TrafficShaperModule::isOverSpeedLimit(int currentSpeed)
{
	float speedLimit = getNum() * 1000000;
	return currentSpeed > speedLimit;
}	

void TrafficShaperModule::delayPacket(Node* current)
{
	float speed_limit = getNum() * 1000000;
	UINT delay = (UINT)((currentTime - lastTime) * (currentSpeed - speed_limit) / speed_limit);
	Sleep(delay);
}

bool TrafficShaperModule::isWindowOverSize(Node* current)
{
	return currentWindow + current->packet.getPacketLen() > WINDOW_SIZE;
}

/*
*	@brief Updates the current window size by adding the packet length to it.
*	
*	The purpose of the current window is to keep track of the amount of data that has been
*	sent in the current period of time, specified by WINDOW_SIZE.
* 
*	@param current The current packet being processed.
*/
void TrafficShaperModule::updateCurrentWindow(Node* current)
{
	currentWindow += current->packet.getPacketLen();
}

/*
*	@brief Updates the time of the last packet processed.
*/
void TrafficShaperModule::updateLastTime()
{
	lastTime = currentTime;
}

void TrafficShaperModule::discardPacket(Node*& current)
{
	Node* poppedNode = packetList.popNode(current);
	delete poppedNode;
}

void TrafficShaperModule::printInfo()
{
	std::stringstream messageStream;
	messageStream << "Limiting packet speed: " << getNum() << "MB/s.";
	sendMessageEx(messageStream);
}

bool TrafficShaperModule::shouldKeepProcessing(Node* node)
{
	return node != packetList.getHead();
}

bool TrafficShaperModule::shouldWork(Node* node)
{
	return checkDirection(node->packet.getPacketAddress()->Outbound, getInbound(), getOutbound());
}

