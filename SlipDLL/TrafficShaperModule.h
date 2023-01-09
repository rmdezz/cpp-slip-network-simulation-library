#pragma once

#include "Module.h"

class TrafficShaperModule : public Module
{
	public:
		TrafficShaperModule(PacketList& packetList);
		~TrafficShaperModule();
		void startUp();
		void closeDown();
		bool process() override;
		bool shouldKeepProcessing(Node* node);
		bool shouldWork(Node* node);
		void printInfo();
		void updateCurrentTime();
		void updateCurrentSpeed(Node*& current);
		void resetCurrentSpeedAndWindowIfNeeded();
		bool isOverSpeedLimit(int currentSpeed);
		void delayPacket(Node* current);
		bool isWindowOverSize(Node* current);
		void discardPacket(Node*& current);
		void updateCurrentWindow(Node* current);
		void updateLastTime();

	private:
		// Set up variables to track the current speed and the current window
		UINT64 currentSpeed;
		UINT64 currentWindow;

		// Set up a variable to hold the current time
		UINT64 currentTime;

		// Set up a variable to hold the time of the last packet
		UINT64 lastTime;
};