#include "DropModule.h"
#include <iostream>
#include "GlobalVar.h"
#include <sstream>
#include "messagePipe.h"
using namespace std;

DropModule::DropModule(PacketList& packetList) : Module(packetList)
{
	setDescription("Randomly discard packets based on probability.");
	setShortName("Drop");
}

DropModule::~DropModule()
{
    
}

void DropModule::startUp()
{
	Module::startUp();
}

bool DropModule::process()
{
	return dropRandomPackets();
}

void DropModule::closeDown()
{
	Module::closeDown();
}

/*
*   @brief Randomly drops packets from the packet list.
*   @return True if at least one packet was dropped, false otherwise.
*/
bool DropModule::dropRandomPackets()
{
    int dropped = 0;

    Node* headC = packetList.getHead();

    while (shouldContinueProcessing(headC->next))
    {
        Node* current = headC->next;
        processCurrentNode(current, headC, dropped);
    }

    return dropped > 0;
}

bool DropModule::shouldContinueProcessing(Node*& current)
{
    return current != packetList.getTail();
}

/*
*   @brief Drops a packet from the packet list if it meets certain criteria.
*   @param current The current packet in the packet list being processed.
*   @param headC The current head node in the packet list.
*   @param dropped The number of packets that have been dropped.
*/
void DropModule::processCurrentNode(Node*& current, Node*& headC, int& dropped)
{
    if (shouldDropPacket(current))
    {
        // Print information about the dropped packet.
        printInfo(current);

        // Remove the packet from the list and delete it
        Node* poppedNode = packetList.popNode(current);
        delete poppedNode;

        // Increment the number of dropped packets.
        dropped++;
    }
    else
    {
        headC = headC->next;
    }
}

/*
*   @brief Prints information about the packet.
*   @param node The dropped packet.
*/
void DropModule::printInfo(Node*& current)
{
    std::stringstream messageStream;
    messageStream << "Dropped with chance " << getChance() << "%";
    sendMessageEx(messageStream);
}

/*
*   @brief Determines whether a packet should be dropped.
*   @param node The packet to be checked.
*   @return True if the packet should be dropped, false otherwise.
*/
bool DropModule::shouldDropPacket(Node*& node)
{
    return checkDirection(node->packet.getPacketAddress()->Outbound, getInbound(), getOutbound())
        && calcChance(getChance());
}


