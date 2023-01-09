#pragma once
#include <unordered_map>
#include <vector>
#include "PacketList.h"
#include "Module.h"
#include "ShuffleList.h"


class ShuffleModule : public Module
{
	public:
		ShuffleModule(PacketList& packetList);
		void startUp();
		void closeDown() override;
		bool process() override;
		bool shufflePackets();
		bool fewPackets();
		bool is_TCP(Node* node);
		bool parsePackets(Node* current, Node* prev, PWINDIVERT_TCPHDR& tcpHeader2,
			PWINDIVERT_IPHDR& ipHeader2, PVOID& payload2, UINT& payloadLen2,
			PWINDIVERT_TCPHDR& tcpHeader, PWINDIVERT_IPHDR& ipHeader, PVOID& payload, UINT& payloadLen);
		bool shouldAddPacketToShuffle(Node* node);
		std::vector<size_t> generateIndices();
		std::vector<size_t> shuffleIndices(const std::vector<size_t>& indices);
		std::vector<Node*> reorderPackets(const std::vector<size_t>& shuffledIndices);
		bool updateSeqNumAndChecksum(std::vector<Node*>& shuffledPackets);
		bool updateTCPHeader(Node* current, Node* prev);
		void addPacketsBackToPacketList(const std::vector<Node*>& shuffledPackets);

	private:
		int packetsStored;
		ShuffleList shuffleList;
		// Create an unordered map with the packet data as the key and the packet as the value
		std::unordered_map<std::string, Node*> packetsMap;
		// Store packets to shuffle in a vector
		std::vector<Node*> packets;
};