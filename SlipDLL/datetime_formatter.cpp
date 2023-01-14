#include "datetime_formatter.h"
#include "GlobalVar.h"

// Define the manipulator as a free function
std::ostream& printDateTimeOS(std::ostream& os)
{
	// Call the function that prints the current date and time
	GlobalVar::GetInstance()->printDateTime();
	// Return the stream object
	return os;
}