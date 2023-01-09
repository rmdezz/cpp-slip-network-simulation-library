#pragma once
#include "LagModule.h"
#include "DropModule.h"
#include "ThrottleModule.h"
#include "DuplicationModule.h"
#include "ShuffleModule.h"
#include "EncryptModule.h"
#include "TrafficShaperModule.h"
#include "Module.h"
#include "PacketList.h"
#include "windivert.h"
#include "Windows.h"

class Divert
{
	public:
		Divert(PacketList& packetList);
		virtual ~Divert();

		void setConsoleTextColor(WORD color);

		int divertStart(const char* filter); //
		bool openDivertHandle(const char* filter, HANDLE& divertHandle); //
		bool setWinDivertParams(HANDLE& divertHandle); //
		void printWinDivertParams(); //
		void resetModules(); // 
		int showError(int errorCode);
		int showDivertOpenError(); // 
		int showSetParamsError();

		bool startThreads(); // 
		bool createMutex(); // 
		bool createThread(LPTHREAD_START_ROUTINE lpStartAddress, Divert* pDivert, const char* threadName); //

		//static DWORD WINAPI memberFunctionWrapper(LPVOID arg); // 
		//static DWORD WINAPI receivePacketsWrapper(LPVOID arg); // 
		//static DWORD WINAPI processPacketsWrapper(LPVOID arg); //

		void processModules(); //
		void startModule(Module*& module); //
		void stopModule(Module*& module); //

		
		void receivePackets(); //
		void handleError(); // 
		bool processPacket(char* packetBuffer, UINT readLength, WINDIVERT_ADDRESS* addressBuffer); //
		bool isPacketTruncated(UINT readLen, UINT maxPacketSize); //
		bool processPacketIfNotStopped(Packet& packet); //
		void free_and_exit(char*& packetBuffer, WINDIVERT_ADDRESS*& addressBuffer); 


		void processPacketsPeriodically();
		void SleepIfNotEnoughTimeElapsed(DWORD startTick, DWORD minTime);
		void CloseDownAllModules();
		void CloseDivertHandle();
		void stopAndSendPackets();

		static const int MODULE_COUNT = 7;

		Module* const* getModules() const;
		std::thread& getReceivePacketsThread();
		std::thread& getProcessPacketsThread();

	private:
		static Module* lagModule;
		static Module* dropModule;
		static Module* throttleModule;
		static Module* duplicatorModule;
		static Module* shuffleModule;
		static Module* encryptModule;
		static Module* trafficShaperModule;
		static Module* modules[MODULE_COUNT];
		PacketList& packetList;
		std::timed_mutex mutex; // control access to processModules and insertPacket functions
		std::thread receivePacketsThread;
		std::thread processPacketsThread;
		bool t1_finished;
		bool t2_finished;

};

