#include "EventsHelper.h"

HANDLE createEvent()
{
	// Create the event
	HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (event == NULL)
	{
		std::string message = "Failed to create the event. Try again.";
		MessageBoxA(NULL,
			message.c_str(),
			"Error",
			MB_ICONERROR | MB_OK);
		return nullptr;
	}
	// Return the handle to the caller
	return event;
}

void waitEvent(HANDLE eventHandle)
{
	DWORD waitResult = WaitForSingleObject(eventHandle, INFINITE);

	if (waitResult != WAIT_OBJECT_0)
	{
		std::string message = "An error ocurred while waiting for the event.";
		MessageBoxA(NULL,
			message.c_str(),
			"Error",
			MB_ICONERROR | MB_OK);
	}

	CloseHandle(eventHandle);
}