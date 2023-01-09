#include "LagList.h"
#include <iostream>
#include "GlobalVar.h"
#include "LagModule.h"
#include <sstream>
#include "messagePipe.h"

#define MAX_BUFFER_LENGTH 2000
#define FLUSH_WHEN_FULL 800

#pragma comment(lib, "winmm.lib")
using namespace std;


LagList::LagList(PacketList& packetList, Module* pModule)
	: packetList(packetList), pModule(pModule)
{
	setName("Lag");
}

LagList::~LagList()
{
	//sendMessage("Lag module's packet list destroyed.");
}

/*
*	@brief Process the packets in the LagList and add delays to them if neccesary.
*	@return True if there are packets still in the LagList, false otherwise.
*/
 bool LagList::processPackets()
 {
	 DWORD currentTime = addPacketsToLagList();
	 releaseReadyPackets(currentTime);
	 if (getLen() >= MAX_BUFFER_LENGTH) flushBuffer();
	 return getLen() > 0;
 }

 /*
 *	@brief	Remove packets from the packet list and add them to the lag list
 *	with a timestamp.
 *	@return The current time when the function is executed in miliseconds.
 */
 DWORD LagList::addPacketsToLagList()
 {
	 DWORD currentTime = timeGetTime();
	 Node* current = packetList.getTail()->prev;

	 while (shouldKeepAddingPackets(current))
	 {
		 if (shouldAddPacket(current))
		 {
			 printInfo();

			 current = packetList.popNode(current);
			 current->packet.setTimestamp(currentTime);

			 addPacketToLagList(current);

			 // Get next packet in packet list
			 current = packetList.getTail()->prev;
		 }
		 else
		 {
			 // Skip the packet
			 current = current->prev;
		 }
	 }

	 return currentTime;
 }

 void LagList::addPacketToLagList(Node* current)
 {
	 insertAfterTargetNode(current, getHead());
	 setLen(getLen() + 1);
 }

 void LagList::printInfo()
 {
	 auto delayTime = pModule->getTime();
	 std::stringstream messageStream;
	 messageStream << "Packet will be delayed for " << delayTime << " ms.";
	 sendMessageEx(messageStream);
 }

 /*
 *	@brief Determine whether the packet in the given node should be added to the lag list.
 *	@param node The node containing the packet to be checked.
 *	@return True if the packet should be added, false otherwise.
 */
 bool LagList::shouldAddPacket(Node* node)
 {
	 return pModule->checkDirection(node->packet.getPacketAddress()->Outbound, pModule->getInbound(), pModule->getOutbound());
 }

 /*
 *	@brief Determine whether the packets should continue to be added to the lag list.
 *	@param node The current node in the packet list being processed.
 *	@return True if packets should continue to be added, false otherwise.
 */
 bool LagList::shouldKeepAddingPackets(Node*& node)
 {
	 return !isBufferFull() && !isPacketListEmpty(node);
 }

 /*
 *	@brief Determine whether the packet list is empty.
 *	@param node The current node in the packet list being processed.
 *	@return True if the packet list is empty, false otherwise.
 */
 bool LagList::isPacketListEmpty(Node*& node)
 {
	 return node == packetList.getHead();
 }

 /*
 *	@brief Determine whether the LagList's buffer is full.
 *	@return True if the buffer is full, false otherwise.
 */
 bool LagList::isBufferFull()
 {
	 return getLen() >= MAX_BUFFER_LENGTH;
 }

/*
*	@brief Determine whether a packet's direction is valid based on the module's inbound
*	and outbound settings.
*	@param packetAddress The packet's address information.
*	@return True if the packet's direction is valid, false otherwise.
*/
 bool LagList::isPacketDirectionValid(WINDIVERT_ADDRESS* packetAddress)
 {
	 return pModule->checkDirection(packetAddress->Outbound, pModule->getInbound(), pModule->getOutbound());
 }

 /*
 *	@brief Releases packets that have been stored in the lag list and are ready to be sent.
 *	@param currentTime The current time in miliseconds.
 */
 void LagList::releaseReadyPackets(DWORD currentTime)
 {
	 // Iterate through the lag list from the end (most recently added packets)
	 Node* current = getTail()->prev;

	 // Send packets that have exceeded their delay time
	 while (shouldSendPacket(current, currentTime))
	 {
		 // Pop the packet from the lag list and insert it into the packet list
		 Node* node = popNode(getTail()->prev);
		 packetList.insertAfterTargetNode(node, packetList.getHead());
		 setLen(getLen() - 1);
	 }
 }

/*
*	@brief Determines if a packet should be sent based on its delay time.
*	@param node The packet's node in the lag list.
*	@param currentTime The current time in miliseconds.
*	@return true if the packet's delay time has been exceeded, false otherwise.
*/
 bool LagList::shouldSendPacket(Node*& node, DWORD currentTime)
 {
	 return !isListEmpty() && isDelayTimeExceeded(node, pModule->getTime(), currentTime);
 }

 bool LagList::isDelayTimeExceeded(Node*& node, float lagTime, DWORD currentTime)
 {
	 return currentTime > node->packet.getTimestamp() + lagTime;
 }

void LagList::flushBuffer()
{
	int flushCount = FLUSH_WHEN_FULL;
	while (shouldFlushPacket(flushCount))
	{
		Node* node = popNode(getTail()->prev);
		packetList.insertAfterTargetNode(node, packetList.getHead());
		setLen(getLen() - 1);
	}
}

bool LagList::shouldFlushPacket(int& flushCount)
{
	return flushCount-- > 0;
}

void LagList::flushList()
{
	sendMessage("Flushing the remaining packets in the buffer");

	std::stringstream messageStream;
	messageStream << "There are a total of " << getLen() << " packets being flushed.";
	sendMessageEx(messageStream);

	List::flushList(packetList);
}