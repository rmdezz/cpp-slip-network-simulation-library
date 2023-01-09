#include <vector>
#include "DuplicationModule.h"
#include "GlobalVar.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <functional>
#include "messagePipe.h"
#include <sstream>
using namespace std;

DuplicationModule::DuplicationModule(PacketList& packetList) : Module(packetList)
{
	setDescription("Send cloned packets immediately after the original packet is sent.");
	setShortName("Duplicator");
	threadsFinished = 0;
}

DuplicationModule::~DuplicationModule()
{
	
}

void DuplicationModule::startUp()
{
	Module::startUp();
}

void DuplicationModule::closeDown()
{
	Module::closeDown();
	threadsFinished = 0;
}

bool DuplicationModule::process()
{
	threadsFinished = 0;
	return duplicatePackets();
}

/*
*	@brief Determines if packets should be duplicated.
*	@return True if any packets were duplicated, false otherwise.
*/
bool DuplicationModule::duplicatePackets()
{
	bool dupplicated = false;

	Node* current = packetList.getHead()->next;

	while (shouldKeepDuplicating(current))
	{
		if (shouldDuplicatePacket(current))
		{
			duplicateCurrentPacket(current);
			dupplicated = true;
		}
		current = current->next;
	}

	return dupplicated;
}

void DuplicationModule::duplicateCurrentPacket(Node*& current)
{
	printInfo();
	createCopies(current);
	insertCopies(current);
}

/*
*	@brief Inserts copies of a packet from the 'copies' queue into the packet list.
*	@param current The current packet being processed.
*/
void DuplicationModule::insertCopies(Node*& current)
{
	Node* actual = current;

	PWINDIVERT_IPHDR ipHeader;
	PWINDIVERT_TCPHDR tcpHeader;
	PVOID payload;

	// If current packet is a TCP packet, store here its sequence number and
	// its payload length
	UINT32 prevSeqNum;
	UINT prevPayloadLen;

	// Check if the current packet is a TCP packet.
	if (isTCP_packet(current))
	{
		parsePacket(current, tcpHeader, ipHeader, payload, prevPayloadLen);
		prevSeqNum = ntohl(tcpHeader->SeqNum);
	}

	// Insert copies from queue into packet list.
	insertPacketsFromQueue(actual, prevSeqNum, prevPayloadLen);
}

/*
*	@brief Inserts packets from the 'copies' queue into the packet list.
*	@param current Pointer to the current packet in the packet list.
*	@param prevSeqNum The previous sequence number of a TCP packet.
*	@param prevPayloadLen The previous payload length of a TCP packet.
*/
void DuplicationModule::insertPacketsFromQueue(Node*& current, UINT32& prevSeqNum, UINT& prevPayloadLen)
{
	bool isTCP = false;

	// Since they are copies of the 'current' packet, there no need to recalculate this value
	// each time in the while loop.
	if (isTCP_packet(current)) isTCP = true;

	while (!copies.empty())
	{
		Node* copy = copies.front();
		packetList.insertBeforeTargetNode(copy, current);

		// Modify sequence number for TCP packets.
		if (isTCP) modifySequenceNumber(copy, prevSeqNum, prevPayloadLen);

		// cout << "Copy packet data: " << copy->packet.getPacket() << endl;
		copies.pop();
		current = copy;
	}
}

/*
*	@brief Modify the TCP sequence number of the packet and recalculate the TCP checksum.
*	@param node A pointer to the node containing the packet to be modified.
*	@param prevSeqNum The previous sequence number to be used to calculate the new sequence number.
*	@param prevPayloadLen The previous payload length to be used to calculate the new sequence number.
*/
void DuplicationModule::modifySequenceNumber(Node*& node, UINT32& prevSegNum, UINT& prevPayloadLen)
{
	// Current node
	UINT64 seqNum;
	UINT ackNum;
	PWINDIVERT_TCPHDR tcpHeader;
	PWINDIVERT_IPHDR ipHeader;
	PVOID payload;
	UINT payload_len;

	// Parse the packet
	bool parsePackets = parsePacket(node, tcpHeader, ipHeader, payload, payload_len);

	if (!parsePackets)
	{
		sendMessage("Error: failed to parse packet");
		//cerr << "Error: failed to parse packet" << endl;
		return;
	}

	/* --- Modify the packet --- */

	updateSequenceNumber(prevSegNum, prevPayloadLen, tcpHeader, payload_len);

	// Recalculate the TCP Checksum
	if (!recalculateChecksum(node))
	{
		sendMessage("Error: Failed to recalculate TCP Checksum");
		//cerr << "Error: Failed to recalculate TCP Checksum" << endl;
		return;
	}
}

void DuplicationModule::updateSequenceNumber(UINT32& prevSegNum, UINT& prevPayloadLen, PWINDIVERT_TCPHDR& tcpHeader, UINT& payloadLen)
{
	// Update the TCP sequence number to previous sequence number plus
	// previous payload length
	tcpHeader->SeqNum = htonl(prevSegNum + prevPayloadLen);

	// Update previous sequence number and payload length with current values
	prevSegNum = ntohl(tcpHeader->SeqNum);
	prevPayloadLen = payloadLen;
}

/*
*	@brief Creates copies of the current packet.
*	@param current The current packet to create copies of.
*/
void DuplicationModule::createCopies(Node*& current)
{
	float numberCopies = getNum();
	std::vector<std::thread> threads;

	// Use threads to create copies of the current packet and add them to the 'copies' queue
	for (int i = 0; i < numberCopies; i++)
	{
		threads.emplace_back(std::bind(&DuplicationModule::duplicatePacket, this, current));
		threads[i].detach();
	}

	while (threadsFinished < numberCopies);
}

void DuplicationModule::duplicatePacket(Node*& current)
{
	//cout << "placing copy in queue" << endl;
	Node* copy = new Node;
	copy->packet = current->packet;
	std::lock_guard<std::mutex> lock(queueMutex);
	copies.emplace(copy);
	threadsFinished++;
}

/*
*	@brief Determines if the current packet should be duplicated.
*	@param current The current packet to consider for duplication.
*	@return True if the current packet should be duplicated, false otherwise.
*/
bool DuplicationModule::shouldDuplicatePacket(Node*& current)
{
	bool dupInbound = getInbound();
	bool dupOutbound = getOutbound();

	return checkDirection(current->packet.getPacketAddress()->Outbound, dupInbound, dupOutbound)
		&& calcChance(getChance());
}

/*
*	@brief Determines if duplication should continue for the remaining packets.
*	@param current The current packet to consider for continuation of duplication.
*	@return True if duplication should continue, false otherwise.
*/
bool DuplicationModule::shouldKeepDuplicating(Node*& current)
{
	return current != packetList.getTail();
}

void DuplicationModule::printInfo()
{
	float copies = getNum();

	std::stringstream messageStream;
	messageStream << "Duplicating packets with a chance of " << getChance() << ".";
	sendMessageEx(messageStream);

	messageStream.str(""); // clear string-stream

	messageStream << copies << " additional copies will be created.";
	sendMessageEx(messageStream);
}