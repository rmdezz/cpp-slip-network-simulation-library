#include "ThrottleList.h"
#include "ThrottleModule.h"
#include "GlobalVar.h"
#include "messagePipe.h"
#include <sstream>
#define THROTTLE_LIMIT 1000

ThrottleList::ThrottleList(PacketList& packetList, Module* pModule) : packetList(packetList), pModule(pModule)
{
	setName("Throttle");
	startTime = 0;
}

ThrottleList::~ThrottleList()
{

}

void ThrottleList::flushList()
{
	sendMessage("Traffic throttling has ended.");
	
	std::stringstream messageStream;
	messageStream << "Releasing all " << getLen() << " packets to the network stack.";
	sendMessageEx(messageStream);

	List::flushList(packetList);
	startTime = 0;
}

bool ThrottleList::throttlePackets()
{
	bool throttled = false;

	if (startTime == 0)
	{
		if (shouldStartThrottling())
		{
			setStartTime();
			throttled = true;
			manageThrottlingPeriod();
		}
	}
	else
	{
		manageThrottlingPeriod();
	}

	return throttled;
}

/*
*	@brief Set the start time of the throttling process.
*/
void ThrottleList::setStartTime()
{
	std::stringstream messageStream;
	messageStream << "Throttling process started with a chance of " << pModule->getChance();
	sendMessageEx(messageStream);

	messageStream.str("");
	messageStream << "Packets will be throttled every " << pModule->getTime() << " ms.";
	sendMessageEx(messageStream);

	startTime = timeGetTime();
}

void ThrottleList::manageThrottlingPeriod()
{
	DWORD currentTime = timeGetTime();
	startThrottlingPeriod(currentTime);
	endThrottlingPeriod(currentTime);
}

/*
*	@brief Processes the packets in the list and throttles them if necessary.
*	@param currentTime The current time in miliseconds
*/
void ThrottleList::startThrottlingPeriod(DWORD& currentTime)
{
	Node* current = packetList.getTail()->prev;
	
	// Iterate through the packets in the list
	while (shouldKeepThrottling(current))
	{
		// Check if the packet should be throttled
		if (shouldThrottlePacket(current))
		{
			throttlePacket(current);

			// Get the most recent packet in packet list to process it.
			current = packetList.getTail()->prev;
		}
		else // Skip the current packet, pass to the previous packet.
			current = current->prev;
	}
}

void ThrottleList::printInfo()
{
	std::stringstream messageStream;
	messageStream << "Throttling packet with a chance of " << pModule->getChance() << ".";
	sendMessageEx(messageStream);
}

bool ThrottleList::shouldStartThrottling()
{
	return !packetList.isListEmpty() && pModule->calcChance(pModule->getChance());
}

bool ThrottleList::shouldKeepThrottling(Node* pac)
{
	return getLen() < THROTTLE_LIMIT && pac != packetList.getHead();
}

void ThrottleList::throttlePacket(Node*& pac)
{
	Node* popped = popNode(pac);
	insertAfterTargetNode(popped, getHead());
	setLen(getLen() + 1);
}


void ThrottleList::endThrottlingPeriod(DWORD& currentTime)
{
	if (shouldStopThrottling(currentTime))
	{
		stopThrottling();
	}
}


bool ThrottleList::shouldThrottlePacket(Node*& node)
{
	bool throttleInbound = pModule->getInbound();
	bool throttleOutbound = pModule->getOutbound();

	return pModule->checkDirection(node->packet.getPacketAddress()->Outbound,
		throttleInbound, throttleOutbound);
}

/*
*	@brief Determines if the ThrottleList should stop the throttling period.
*	@param currentTick The current time.
*	@return True if the number of packets in the ThrottleList is greater than or equal to the
*	ThrottleList limit or if the current time is greater than the startTime + the throttling frame.
*/
bool ThrottleList::shouldStopThrottling(DWORD& currentTime)
{
	return getLen() >= THROTTLE_LIMIT || shouldEndThrottling(currentTime, startTime, pModule->getTime());
}

/*
*	@brief Check if the throttling process should end.
*	@param currentTime The current time.
*	@param startTime The start time of the throttling process.
*	@param throttleFrame The time frame for the throttling process.
*	@return True if the current time is greater than the start time + the throttling frame.
*/
bool ThrottleList::shouldEndThrottling(DWORD& currentTime, DWORD& startTime, float throttleFrame)
{
	return currentTime - startTime > throttleFrame;
}

void ThrottleList::stopThrottling()
{
	bool dropThrottled = pModule->getDrop();

	if (dropThrottled) dropPackets();
	else flushList();
}

/*
*	@brief Drop all packets in the ThrottleList.
*	@details Print a message to the console saying that the throttling period has ended
*	and the number of packets being dropped.
*	Check if the ThrottleList reached its maximum capacity and print a message indicating this.
*	Finally, iterate through the ThrottleList and delete all the nodes in it. Reset the start
*	time to 0.
*/
void ThrottleList::dropPackets() 
{
	printDroppedPacketsInfo();
	
	printThrottleCapacityInfo();
	
	deleteAllPackets();

	resetStartTime();
}

void ThrottleList::printDroppedPacketsInfo()
{
	std::stringstream messageStream;
	messageStream << "Throttling ended, dropping " << getLen() << " packets from buffer.";
	sendMessageEx(messageStream);
}

void ThrottleList::printThrottleCapacityInfo()
{
	if (getLen() == THROTTLE_LIMIT)
		sendMessage("Buffer reached maximum capacity? TRUE");
	else
		sendMessage("Buffer reached maximum capacity? FALSE");
}

void ThrottleList::deleteAllPackets()
{
	while (!isListEmpty())
	{
		delete popNode(getTail()->prev);
		setLen(getLen() - 1);
	}
}

void ThrottleList::resetStartTime()
{
	startTime = 0;
}