// GlobalVariablesSingleton

/*
 * In general, it is generally a good practice to include the necessary
 * header files in your code rather than relying on typedefs or other
 * workaround solutions, as it can help to ensure that the code is
 * correct and optimized for performance.
 */

#pragma once
#include <mutex>
#include "windows.h"

class GlobalVar
{
private:
	static GlobalVar* instance;
	static std::mutex instanceMutex;
	GlobalVar();
	
public:
	static GlobalVar* GetInstance();
	static void DeleteInstance();
	void printDateTime();
	void printMessage(const char* message);

	// Declare global variables here
	HANDLE loopThread;
	HANDLE clockThread;
	HANDLE mutex;
	HANDLE divertHandle;
	volatile bool stopLooping = false;
};

