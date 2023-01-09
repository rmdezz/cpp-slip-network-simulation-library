#include "GlobalVar.h"
#include <iostream>
#include <iomanip>
#include "messagePipe.h"
using namespace std;

// Initialize static member variables
GlobalVar* GlobalVar::instance = nullptr;
std::mutex GlobalVar::instanceMutex;

GlobalVar::GlobalVar()
{
	// Initialize global variables here
	stopLooping = false;
}

GlobalVar* GlobalVar::GetInstance()
{
	/*
	 * By using the double-checked locking pattern, the GetInstance method can
	 * ensure that only one instance of the singleton class is created, while
	 * also minimizing the overhead of acquiring the lock.
	 */

	if (instance == nullptr)
	{
		std::lock_guard<std::mutex> lock(instanceMutex);
		if (instance == nullptr)
		{
			instance = new GlobalVar();
		}
	}
	return instance;
}

void GlobalVar::DeleteInstance()
{
	std::lock_guard<std::mutex> lock(instanceMutex);
	delete instance;
	//sendMessage("GlobalVar singleton destroyed.");
	instance = nullptr;
}

void GlobalVar::printDateTime()
{
	// Get current time
	std::time_t currentTime = std::time(nullptr);
	// Convert time to tm structure
	std::tm localTime;
	localtime_s(&localTime, &currentTime);
	// Print local time
	char buffer[26];
	asctime_s(buffer, sizeof(buffer), &localTime);
	buffer[24] = '\0';
	cout << "[" << buffer << "]" << setw(8) << " ";
}

void GlobalVar::printMessage(const char* message)
{
	GlobalVar::GetInstance()->printDateTime();
	cout << message << endl;
}