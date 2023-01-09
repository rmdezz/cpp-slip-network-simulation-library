#include "Module.h"
#include <iostream>
#include <mutex>
#include "Divert.h"
#include "GlobalVar.h"
#include <cstring>
#include "messagePipe.h"
#include <sstream>

#define MAX_PACKET_SIZE 0xFFFF
//PacketList Module::packetList;

Module::Module(PacketList& packetList) : packetList(packetList)
{
	displayName = nullptr;
	shortName = nullptr;
	enabledFlag = false;
	lastEnabled = false;
	processTriggered = false;
	chance = 45;
	enabledChance = false;
	inbound = true;
	outbound = true;
	state = 12345;
	time = 0;
	num = 0;
	drop = false;
}

Module::~Module()
{
	//std::stringstream messageStream;
	//messageStream << getShortName() << " module destroyed.";
	//sendMessageEx(messageStream);
	if (displayName != nullptr) delete[] this->displayName;
	if (shortName != nullptr) delete[] this->shortName;
}

void Module::setDescription(const char* displayName)
{
	if (this->displayName != nullptr) delete[] this->displayName;
	this->displayName = new char[strlen(displayName) + 1];
	strcpy_s(this->displayName, strlen(displayName) + 1, displayName);
}

void Module::setShortName(const char* shortName)
{
	if (this->shortName != nullptr) delete[] this->shortName;
	this->shortName = new char[strlen(shortName) + 1];
	strcpy_s(this->shortName, strlen(shortName) + 1, shortName);
}

bool Module::getEnabledFlag()
{
	return this->enabledFlag;
}

void Module::setEnabledFlag(bool enabledFlag)
{
	this->enabledFlag = enabledFlag;
}

bool Module::getLastEnabled()
{
	return this->lastEnabled;
}

void Module::setLastEnabled(bool lastEnabled)
{
	this->lastEnabled = lastEnabled;
}

char* Module::getShortName()
{
	return this->shortName;
}

/*
 * Note that static methods don't have access to the non-static member variables and methods of the class.
 * If you need to access these variables or methods, you can either make them static as well or pass them
 * as arguments to the static method.
 */

/* "LPVOID" is more general purpose and can be used to refer to any type of data. */

bool* Module::getProcessTrigerred()
{
    return &(this->processTriggered);
}

void Module::setProcessTriggered(bool processTriggered)
{
    this->processTriggered = processTriggered;
}

void Module::startUp()
{
	std::stringstream messageStream;
	messageStream << getShortName() << " module initialized successfully.";
	std::string finalMessage = messageStream.str();
	sendMessage(finalMessage.c_str());
	//cout << getShortName() << " module initialized successfully!" << endl;
}


bool Module::checkDirection(bool outboundPacket, bool handleInbound, bool handleOutbound)
{
	return (handleInbound && !outboundPacket) || (handleOutbound && outboundPacket);
}

bool Module::calcChance(float chance) 
{
	// Generate a random number in the range 0 to 2^32-1 using the XORShift32 algorithm
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;

	// Scale the random number to the desired range
	// then compare it with the chance
	// and return the result
	return state % 101 <= chance;
}

void Module::setChance(float chance)
{
	this->chance = chance;
}

float Module::getChance()
{
	return this->chance;
}

bool Module::getInbound()
{
	return this->inbound;
}

bool Module::getOutbound()
{
	return this->outbound;
}

void Module::setInbound(bool inbound)
{
	this->inbound = inbound;
}

void Module::setOutbound(bool outbound)
{
	this->outbound = outbound;
}

bool Module::parsePacket(Node*& packetNode, PWINDIVERT_TCPHDR& tcpHeader,
	PWINDIVERT_IPHDR& ipHeader, PVOID& payload, UINT& payload_len)
{
	return WinDivertHelperParsePacket(packetNode->packet.getPacket(),
		packetNode->packet.getPacketLen(), &ipHeader, NULL, NULL,
		NULL, NULL, &tcpHeader, NULL, &payload, &payload_len,
		NULL, NULL);
}

bool Module::isTCP_packet(Node*& node)
{
	UINT8 protocol;

	if (!WinDivertHelperParsePacket(node->packet.getPacket(), node->packet.getPacketLen(),
		NULL, NULL, &protocol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
	{
		sendMessage("ERROR: Failed to parse packet");
		//cout << "ERROR: Failed to parse packet" << endl;
		return false;
	}

	return protocol == IPPROTO_TCP;
}

bool Module::recalculateChecksum(Node* packetNode)
{
	return WinDivertHelperCalcChecksums(packetNode->packet.getPacket(),
		packetNode->packet.getPacketLen(), packetNode->packet.getPacketAddress(), 0);
}

void Module::closeDown()
{
	std::stringstream messageStream;
	messageStream << getShortName() << " module disabled.";
	std::string finalMessage = messageStream.str();
	sendMessage(finalMessage.c_str());
	//cout << getShortName() << " module disabled." << endl;
}

float Module::getTime()
{
	return this->time;
}

void Module::setTime(float time)
{
	this->time = time;
}

float Module::getNum()
{
	return this->num;
}

void Module::setNum(float num)
{
	this->num = num;
}

bool Module::getDrop()
{
	return this->drop;
}

void Module::setDrop(bool drop)
{
	this->drop = drop;
}