#include <random>
#include "ShuffleModule.h"
#include <string>
#include <algorithm>
#include <numeric> // std::iota
#include "messagePipe.h"

typedef std::mt19937 random;
//#define NEED_AT_LEAST 15

using namespace std;

ShuffleModule::ShuffleModule(PacketList& packetList) : Module(packetList), shuffleList(packetList, this)
{
	setDescription("Rearranges the order of packets");
	setShortName("Shuffle");
	packetsStored = 0;
}

void ShuffleModule::startUp()
{
	Module::startUp();
}

void ShuffleModule::closeDown()
{
	Module::closeDown();
}

/*
*	@brief Processes the packets in the packet list to shuffle them.
*	@return True if the packets were shuffled successfully, false otherwise.
*/
bool ShuffleModule::process()
{
	if (!packetList.isListEmpty())
	{
		Node* current = packetList.getTail()->prev;
		float minPackets = getNum();
		if (packets.size() < minPackets && shouldAddPacketToShuffle(current))
		{
			current = packetList.popNode(current);
			packets.push_back(current);
			return false;	
		}
		else
		{
			return shufflePackets();
		}
	}
		
	return false;
}

bool ShuffleModule::shouldAddPacketToShuffle(Node* node)
{
	return checkDirection(node->packet.getPacketAddress()->Outbound, getInbound(), getOutbound())
		&& calcChance(getChance());
}

bool ShuffleModule::is_TCP(Node* node)
{
	UINT8 protocol;
	WinDivertHelperParsePacket(node->packet.getPacket(), node->packet.getPacketLen(), NULL, NULL, &protocol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	return protocol == IPPROTO_TCP;
}

/*
*	@brief Shuffles the gathered packets and updates the sequence numbers and
*	checksums of the TCP packets.
*	@return true if the packets were shuffled successfully, false otherwise.
*/
bool ShuffleModule::shufflePackets()
{
	sendMessage("Shuffling gathered packets.");
	
	// Create a vector of indices to represent the original order of the packets
	std::vector<size_t> indices = generateIndices();

	// Shuffle the indices using a random number generator
	std::vector<size_t> shuffledIndices = shuffleIndices(indices);

	// Reorder the packets according to the shuffled indices
	std::vector<Node*> shuffledPackets = reorderPackets(shuffledIndices);

	// Update seq num and checksum of the shuffled (TCP) packets

	/*
	*	Sequence number:
	*	The sequence number of a TCP packet is a 32-bit field in the TCP header
	*	that specifies the order in which the packet is sent. It helps the receiver
	*	to put the packets in the correct order and to detect missing packets.
	* 
	*	Checksum:
	*	The checksum of a TCP packet is a 16-bit field in the TCP header that is used
	*	to verify the integrity of the packet. It helps to ensure that the packet
	*	has not been tampered with or corrupted during transmission.
	*/
	if (!updateSeqNumAndChecksum(shuffledPackets))
		return false;

	addPacketsBackToPacketList(shuffledPackets);
	
	shuffledPackets.clear();
	packets.clear();
	sendMessage("Packets shuffled successfully.");

	return true;
}

std::vector<size_t> ShuffleModule::generateIndices()
{
	std::vector<size_t> indices(packets.size());
	std::iota(indices.begin(), indices.end(), 0);
	return indices;
}

std::vector<size_t> ShuffleModule::shuffleIndices(const std::vector<size_t>& indices)
{
	std::random_device seed;
	random rnd(seed());
	auto shuffledIndices = indices;
	std::shuffle(shuffledIndices.begin(), shuffledIndices.end(), rnd);
	return shuffledIndices;
}

std::vector<Node*> ShuffleModule::reorderPackets(const std::vector<size_t>& shuffledIndices)
{
	std::vector<Node*> shuffledPackets(packets.size());
	for (size_t i = 0; i < shuffledIndices.size(); i++)
	{
		shuffledPackets[i] = packets[shuffledIndices[i]];
	}
	return shuffledPackets;
}

bool ShuffleModule::updateSeqNumAndChecksum(std::vector<Node*>& shuffledPackets)
{
	sendMessage("Updating sequence number and checksum for TCP packets.");

	int len = shuffledPackets.size();
	int dp = -1;

	for (int i = len - 1; i >= 0; i--)
	{
		Node* current = shuffledPackets[i];
		
		if (isTCP_packet(current))
		{
			if (dp > i && dp >= 0)
			{
				Node* prev = shuffledPackets[dp];
				if (!updateTCPHeader(current, prev))
				{
					shuffledPackets.clear();
					packets.clear();
					return false;
				}
			}

			dp = i;
		}
	}

	return true;
}

/*
*	@brief Update the sequence number and checksum for a TCP packet.
*	@param current The current packet node to update.
*	@param prev The previous packet to use as reference.
*	@return tre if the update was successful, false otherwise.
*/
bool ShuffleModule::updateTCPHeader(Node* current, Node* prev)
{
	PWINDIVERT_IPHDR ipHeader2;
	PWINDIVERT_TCPHDR tcpHeader2;
	PVOID payload2;
	UINT payloadLen2;

	PWINDIVERT_IPHDR ipHeader;
	PWINDIVERT_TCPHDR tcpHeader;
	PVOID payload;
	UINT payloadLen;

	if (!parsePackets(current, prev, tcpHeader2, ipHeader2, payload2, payloadLen2,
		tcpHeader, ipHeader, payload, payloadLen))
	{
		sendMessage("Error: failed to parse packets.");
		return false;
	}

	// Set the TCP sequence number
	tcpHeader2->SeqNum = htonl(ntohl(tcpHeader->SeqNum) + payloadLen);

	// Re-calculate the TCP Checksum
	if (!recalculateChecksum(current))
	{
		sendMessage("Error: failed to recalculate TCP checksum.");
		return false;
	}

	return true;
}

/*
*	@brief Add the packets back to the packet list.
*	@param shuffledPackets The packets to be added back to the packet list.
*/
void ShuffleModule::addPacketsBackToPacketList(const std::vector<Node*>& shuffledPackets)
{
	sendMessage("Sending packets back to the packet list...");
	for (auto packet : shuffledPackets)
	{
		packetList.addNodeBeforeTail(packet);
	}
}

bool ShuffleModule::fewPackets()
{
	return packetList.getSize() <= 2;
}

bool ShuffleModule::parsePackets(Node* current, Node* prev, PWINDIVERT_TCPHDR& tcpHeader2,
	PWINDIVERT_IPHDR& ipHeader2, PVOID& payload2, UINT& payloadLen2,
	PWINDIVERT_TCPHDR& tcpHeader, PWINDIVERT_IPHDR& ipHeader, PVOID& payload, UINT& payloadLen)
{
	if (!parsePacket(current, tcpHeader2, ipHeader2, payload2, payloadLen2) || !parsePacket(prev, tcpHeader, ipHeader, payload, payloadLen))
	{
		sendMessage("Error: failed to parse packets.");
		//cerr << "ERROR: Failed to parse packets." << endl;
		return false;
	}
	return true;
}
