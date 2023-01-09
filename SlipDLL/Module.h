#pragma once
#include "PacketList.h"
#include <iostream>
#include "Node.h"
using namespace std;

class Module
{
private:
	char* displayName;
	char* shortName;
	bool enabledFlag;
	bool lastEnabled; 
	bool processTriggered;
	float chance;
	bool enabledChance;
	bool inbound;
	bool outbound;
	float time; // in miliseconds (drop and lag module)
	float num; // Only used for the duplicate (num. copies) and traffic shaper (num = speed_limit) modules
	bool drop; // Only used for the drop module

	/*  "HANDLE" is typically used to refer to objects that are managed
		 *  by the operating system, such as files, pipes, and threads*/
	

public:
	Module() = delete;
	Module(PacketList& packetList);
	virtual ~Module();

	void setDescription(const char* displayName);
	
	void setShortName(const char* shortName);
	char* getShortName();

	bool getEnabledFlag();
	
	void setEnabledFlag(bool enabledFlag);
	
	bool* getProcessTrigerred();
	void setProcessTriggered(bool processTriggered);
	
	bool getLastEnabled();
	void setLastEnabled(bool lastEnabled);
	
	float getChance();
	void setChance(float chance);
	
	float getTime();
	void setTime(float time);
	
	float getNum();
	void setNum(float num);
	
	bool getDrop();
	void setDrop(bool drop);

	bool getInbound();
	void setInbound(bool inbound);
	
	bool getOutbound();
	void setOutbound(bool outbound);

	virtual void closeDown();
	virtual void startUp();
	virtual bool process() = 0;

	bool checkDirection(bool outboundPacket, bool handleInbound, bool handleOutbound);
	bool calcChance(float chance);
	bool parsePacket(Node*& packetNode, PWINDIVERT_TCPHDR& tcpHeader,
		PWINDIVERT_IPHDR& ipHeader, PVOID& payload, UINT& payload_len);
	bool isTCP_packet(Node*& node);
	bool recalculateChecksum(Node* packetNode);

protected:
	PacketList& packetList;
	uint32_t state;
};
