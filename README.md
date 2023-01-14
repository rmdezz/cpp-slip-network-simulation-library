
# Network simulation library

Slip is a robust and feature-rich network simulation tool
developed in C/C++. It empowers users with the ability to test
and debug their network applications in various simulated
conditions. The tool comprises of various modules such as lag,
drop, throttle, duplicate, shuffle, and traffic shaper,
which can be seamlessly configured to mimic different network
scenarios. The Slip library is designed to be user-friendly,
easy to integrate into existing projects, and comes with a simple
yet powerful interface for applying and adjusting the different
simulation modules. Whether you're building a new network
application or looking to test the behavior of your existing
software under varying conditions, cpp-slip can prove to be an
invaluable tool in your arsenal.










## Getting Started

1. **Downloading the pre-compiled binary:**
Go to the Releases section of the Github repository, and
download the pre-compiled binary of the project along with
any dependencies.
     
     **This is what you will get from the "Release" package:**
- SlipDLL.dll
- SlipDLL.lib
- NetSimulatorAPI.h
- SlipClient.exe
- WinDivert.dll
- WinDivert64.sys
- libcrypto-3-x64.dll
- PipeReader.dll
- pipe_reader.h

2. **Using the library:**
Once you have the binary, you can include it in your own
project by adding the appropriate include path to your code.

`Note:` When using a pre-compiled DLL in a C or C++ project, 
you typically need to include the DLL, along with 
its associated header file (.h) and import library (.lib) file 
in your project. 

#### Option 1: Using the library in a C/C++ project:

Here's how you can include the DLL, header file,
and import library in your C or C++ project:

   - Create two new folders in the root of your project called "include" and "lib", and inside of each of them create a folder called "slip".
   - Copy the DLL **(SlipDLL.dll)**, header file **(NetSimulatorAPI.h)**, and import library files **(SlipDLL.lib)** to the appropriate folders: 
     - The DLL file **should be placed in a directory that is in the system's PATH or in the same directory as the executable that will use the DLL.**
     - The header file should be copied to the "include" folder. 
     - The import library file should be copied to the "lib" folder.

   - In your project settings, add the "include" and "lib" folders to the include and library search directories, respectively.
   - In Visual Studio, you can do this by going to the "Project" menu, selecting "Properties", and then adding the include and library paths under "C/C++" > "General" > "Additional Include Directories" and "Linker" > "General" > "Additional Library Directories" respectively.
     ```
     Additional Include Directories
     -------------------------------
     include/slip

     Additional Library Directories
     -------------------------------
     lib/slip
     ```

   - In the linker settings of your project, add the import library file as an additional dependency:
     - In Visual Studio, you can do this by going to the "Project"menu, selecting "Properties", and then adding the import library file under "Linker" > "Input" > "Additional Dependencies":

       ```
       Additional Dependencies
       -----------------------
        SlipDLL.lib
       ```

   -  Press Apply and OK button, to save the changes

   - In your C or C++ code, include the DLL's header file using  the #include preprocessor directive:
     ```
     #include "NetSimulatorAPI.h"
     ```

   - Rebuild the project.

#### Option 2: Using the library in a C# project
- Set the DLL directory using the following code snippet at the beginning of the program before calling any DLL functions:
  ```
  [DllImport("kernel32.dll")]
  private static extern bool SetDllDirectory(string lpPathName);

  static void Main(string[] args)
  {
    SetDllDirectory(Directory.GetCurrentDirectory());
  }
  ```
- Add the Slip library (.dll) to the project by placing it in the output directory of the C# project or configuring the project's build settings to copy the DLL to the output directory when the project is built.
- Define the extern "C" functions by using the `DllImport` attribute, specifying the path to the DLL. For example:
  ```js
  [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)] 
  public static extern void startSimulationEx
  ([MarshalAs(UnmanagedType.LPStr)] string filterText,
   [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.Struct)] Config[] configs,
    int configCount, IntPtr eventHandle);

  [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
  public static extern void startSimulation([MarshalAs(UnmanagedType.LPStr)] string filterTex,
   IntPtr eventHandle);  

  [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
  public static extern IntPtr createEvent();

  [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
  public static extern void waitEvent(IntPtr eventHandle);

  [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
  public static extern void enableLag();

  [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
  public static extern void stopSimulation();
  ```
- Create an enum in C# that matches the enum in C++, in this case, it would be the SimulationType enum.
  ```
  public enum SimulationType
  {
    Lag,
    Drop,
    Throttle,
    Duplicate,
    Shuffle,
    Encrypt,
    TrafficShaper
  }
  ```
- Create struct in C# that matches the struct in C++, in this case, it would be the Config struct.
  ```
  public struct Config
  {
    [FieldOffset(0)]
    public bool enabled;
    [FieldOffset(1)]
    public bool inbound;
    [FieldOffset(2)]
    public bool outbound;
    [FieldOffset(3)]
    public float chance;
    [FieldOffset(7)]
    public float delay;       
    [FieldOffset(11)]
    public float num;
    [FieldOffset(15)]
    public bool drop;
    [FieldOffset(16)]
    public SimulationType simulationType;
  }
  ```

3. **Configuring the named pipe client**
To get the most out of the Slip network simulation tool,
it's important to understand how to communicate with the
underlying library. One key aspect of this is receiving
updates and messages from the library, which can include errors,
progress updates, and the current state of the simulation.
To facilitate this communication, the Slip library uses a named
pipe to send messages to a separate client.

- Option 1: The simplest way
  
  The simplest way is to use the provided SlipClient.exe executable,
  which can be found in the same folder as the Slip.dll library. This
  executable will automatically connect to the named pipe and display
  any messages received in a separate window.

  You **MUST** run this executable before starting the simulation
  in order to receive the messages from the start.

  This is how you can do it through code: 
  (SlipClient.exe and PipeReader.dll must
  be in the same folder as your project executable).

  In C/C++:
  ```
  #include <stdio.h>
  #include <stdlib.h>
  #include <Windows.h>
  #include "NetSimulatorAPI.h"

  int main()
  {
    __declspec(dllimport) HANDLE startPipeClient();
    __declspec(dllimport) void stopPipeClient(HANDLE processHandle);

    // To start the pipe client
    HANDLE pipeClient = startPipeClient();
    
    // To stop the pipe client
    // Usually you will want to call this function before closing your program.
    stopPipeClient(pipeClient);
  }
  ```

  In C#:
    ```
    public class PipeClient
    {
        [DllImport("PipeReader.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr startPipeClient();

        [DllImport("PipeReader.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void stopPipeClient(IntPtr processHandle);

        private IntPtr processHandle;

        public void Start()
        {
            processHandle = startPipeClient();
        }

        public void Stop()
        {
            stopPipeClient(processHandle);
        }
    }

    static void Main(string[] args)
    {
        PipeClient pipeClient = new PipeClient();

        // Start the pipe client
        pipeClient.Start();
        
        // other code
        ...

        // To stop the pipe client
        // Usually you will want to call this function before closing your program.
        pipeClient.Stop();
    }
    ```
- Option 2: Create your own client using the PipeReader.dll library.
  With this approach, you have more control over how the messages are displayed and handled,
  but you'll need to write additional code to handle the communication with the pipe.

  In C:
  ```js
  #include "pipe_reader.h"

  DWORD WINAPI ReadFromPipeThread(LPVOID lpParam)
  {
    __declspec(dllimport) char* ReadFromPipe();
    // Cast the LPVOID lpParam to a pointer to a shared variable
    bool* keepRunning = (bool*)lpParam;

    while (*keepRunning)
    {
        // You can choose how you want to display
        // the message, such as displaying it in a
        // console or using a component within a UI
        // application.
        char* message = ReadFromPipe();
        
        // Print the message
        std::cout << message << std::endl;

        // Free the memory allocated by the ReadFromPipe function
        GlobalFree(message);
    }

    return 0;
  }

  int main()
  {

    // Create a shared variable
    bool keepRunning = true;

    // Create thread for the ReadFromPipeThread and run inmediately
    // Pass the address of the shared variable as an argument to the
    // ReadFromPipeThread function
    HANDLE pipeHandle = CreateThread(NULL, 0, ReadFromPipeThread, &keepRunning, 0, NULL);

    // Other code
    ...

    /* To stop the thread */
    // Signal the ReadFromPipeThread to stop running
    keepRunning = false;
    // Wait for the thread to finish
    WaitForSingleObject(pipeHandle, INFINITE);
    // Close resource
    CloseHandle(pipeHandle);

    return 0;
  }
  ```

  In C++:
  ```js
  #include <thread>

  void ReadPipeThread(bool* keepRunning)
  {
      __declspec(dllimport) char* ReadFromPipe();

      while (*keepRunning)
      {
          char* message = ReadFromPipe();
          std::cout << message << std::endl;
          GlobalFree(message);
      }
  }

  int main()
  {
      // Start the thread
      bool keepRunning = true;
      std::thread pipeThread(ReadPipeThread, &keepRunning);
      pipeThread.detach();

      // Do some other work
      ...

      // To stop the thread
      keepRunning = false;

      // Be sure that the thread has finished
      // and it's safe to release the resources.
      pipeThread.join(); 
      
  }
  ```

  In C#:
  ```js
  using System.Threading;
  using System.Runtime.InteropServices;

  private static volatile bool keepRunning = true;
  private static Thread pipeThread;

  [DllImport("PipeReader.dll", CharSet=CharSet.Ansi)]
  private static extern IntPtr ReadFromPipe();

  static void ReadFromPipeThread()
  {
      while (keepRunning)
      {
          IntPtr message = ReadFromPipe();
          string message = Marshal.PtrToStringAnsi(messagePtr);
          if (message == null) continue;
          
          // Print the message
          Console.WriteLine(message);

          // Free the memory allocated by the ReadFromPipe function
          Marshal.FreeHGlobal(messagePtr);

          Thread.Sleep(50);
      }
  }

  static void Main(string[] args)
  {
      // Create the thread to run the ReadFromPipe function
      pipeThread = new Thread(new ThreadStart(ReadFromPipeThread));
      // Start the thread
      pipeThread.Start();

      // Do some other work
      ...

      // Set the keepRunning to false to stop the thread
      keepRunning = false;

      // Wait for the thread to complete
      thread.Join();
  }
    

4. **Starting a simulation:**
To create a simulation, you will need to first create an 
instance of the Config struct and set its fields to configure
the simulation. 

You can see the declaration of the Config and SimulationType in the NetSimulatorAPI.h file:
```
enum SimulationType
{
    Lag,
    Drop,
    Throttle,
    Duplicate,
    Shuffle,
    Encrypt,
    TrafficShaper
};

struct Config
{
    bool enabled;
    bool inbound;
    bool outbound;
    float chance;
    float delay;   
    float num;   
    bool drop; 
    SimulationType type;
};
```

For example, to create a simulation 
with a lag of 1000ms for the traffic from/to a specific ip:

This is how you can create an instance of the `Config` struct and fill it
with the appropriate values for the lag module.

- The `enabled` field should be set to "true" to enable the lag module.
- The `inbound` and `outbound` fields should be set to `true` or `false` depending on whether you want to apply the lag to inbound or outbound packets, or both.
- The `chance` field should be set to `0` as it' not used in the lag module.
- The `delay` field should be set to the desired delay time in miliseconds.
- The `num` field should be set to `0` as it' not used in the lag module.
- The `drop` field should be set to `false` as it's not used in the lag module.
- The `type` field should be set to `SimulationType::Lag` to indicate that this configuration is for the lag module.

In C/C++:
```
Config configs[1];
configs[0] = { true, true, true, 0, 1000, 0, false, SimulationType::Lag };

const char* filter = "ip.DstAddr == 140.82.114.3";

```

In C#:
```
Config lagConfig = new Config
{
    enabled = true,
    inbound = true,
    outbound = true,
    chance = 0,
    delay = 500,
    num = 0,
    drop = false,
    simulationType = SimulationType.Lag
};

...

List<Config> configList = new List<Config>();

// Add config objects to configList
configList.Add(lagConfig);
configList.Add(traficShaperConfig);

// Get configs in an array format
Config[] configArray = configList.ToArray();

string filter = "ip.DstAddr == 140.82.114.3";

```

#### To start the simulation you have two options:

- Option 1: The simplest way. Using the helper functions provided:
  
  **Reminder:** You can also use these functions to modify the simulation settings without having to stop and start a new simulation.
  - Call the `startSimulation` function, passing in the following parameters:
    - A `const char*` filter text, used to specify which packets to capture and apply the simulation to, you can find the filter syntax on the WinDivert documentation. [Filter language syntax](https://reqrypt.org/windivert-doc.html#filter_language)
    - A `HANDLE` event, which will be used to notify you when the simulation has started.
  - Call the appropriate functions to enable/configure the module you want to use.
    In this case, it would be `SetLag`, `enableLag`, `disableLag`, `setLagDelay`, `setLagInbound`, etc.
    You can see all the functions available for the modules in the NetSimulatorAPI.h file.

    In C:
    ```js
    #include "NetSimulatorAPI.h"

    DWORD WINAPI startSimulationThread(LPVOID lpParam)
    {
        __declspec(dllimport) void startSimulation(const char* filterText, HANDLE eventHandle);

        // Cast the LPVOID argument back to an array of void pointers
        void** args = (void**)lpParam;

        // Extract the filter and eventHandle from the array
        const char* filter = (const char*)args[0];
        HANDLE eventHandle = (HANDLE)args[1];

        // Start the simulation using the filter and eventHandle
        startSimulation(filter, eventHandle);

        return 0;
    }

    int main()
    {
        // Import the necessary functions
        __declspec(dllimport) HANDLE createEvent();
        __declspec(dllimport) void waitEvent(HANDLE event);
        __declspec(dllimport) void enableLag();
        __declspec(dllimport) void setLagDelay(float delay);
        ....
        __declspec(dllimport) void stopSimulation();

        // Create an event handle to be used in the simulation
        HANDLE eventHandle = createEvent();

        // Configure parameters
        const char* filter = "ip.DstAddr == 140.82.114.3";

        // Create thread to handle the startSimulationThread function and run inmediately in background
        HANDLE threadHandle = CreateThread(NULL, 0, startSimulationThread, args, 0, NULL);

        // Wait for the event (initialization of the simulation)
        waitEvent(eventHandle);

        // Configure the modules you want to use

        // First option:
        SetLag(true, true, true, 1000);
        SetDrop(true, true, true, 80.45);

        // Second option:
        enableLag();
        setLagDelay(1000);
        setLagInbound(true);
        setLagOutbound(true);

        enableDrop();
        setDropInbound(true);
        setDropOutbound(true);
        setDropChance(80.45);

        ....

        // Continue with your code
        ....

    }

    ```

    In C++:
    ```js
    #include "NetSimulatorAPI.h"
    #include <thread>

    int main()
    {
        // Import the necessary functions
        __declspec(dllimport) HANDLE createEvent();
        __declspec(dllimport) void waitEvent(HANDLE event);
        __declspec(dllimport) void enableLag();
        __declspec(dllimport) void setLagDelay(float delay);
        __declspec(dllimport) void stopSimulation();
        __declspec(dllimport) void startSimulation(const char* filterText, HANDLE eventHandle);

        // Create an event handle to be used in the simulation
        HANDLE eventHandle = createEvent();

        // Configure parameters
        const char* filter = "ip.DstAddr == 140.82.114.3";
        ...

        // Create thread
        std::thread startThread(startSimulation, "ip.DstAddr == 140.82.113.4", eventHandle);
        startThread.detach(); // Run in background

        // Wait for the event (initialization of the simulation)
        waitEvent(eventHandle);

        // First option:
        SetLag(true, true, 1000);
        SetDrop(true, true, true, 80.45);

        // Second option:
        enableLag();
        setLagDelay(1000);
        setLagInbound(true);
        setLagOutbound(true);

        enableDrop();
        setDropInbound(true);
        setDropOutbound(true);
        setDropChance(80.45);

        ...

        // Continue with your code
        ...
    }
    ```

    In C#:
    ```js
    using System;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.Collections.Generic;
    using System.IO;

    [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void startSimulation([MarshalAs(UnmanagedType.LPStr)] string filterTex,
     IntPtr eventHandle);

    [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetLag(bool enabled, bool inbound, bool outbound, float delay);

    [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void enableLag();

    [DllImport("SlipDLL.dll", CallingConvention = CallingConvention.Cdecl)]
    public static exern void setLagDelay(float delay);
    ...

    static void Main(string[] args)
    {
        ...

        IntPtr eventHandle = createEvent();
        new Thread(() => startSimulation("ip.DstAddr == 140.82.114.3", eventHandle)).Start();
        waitEvent(eventHandle);

        // First option:
        SetLag(true, true, 1000);
        SetDrop(true, true, true, 80.45);

        // Second option:
        enableLag();
        setLagDelay(1000);
        
        enableDrop();
        setDropChance(80.45);

        // Continue with your code
        ...
    }
    ```

- Option 2: Using the "startSimulationEx" function:
  - Call the 'startSimulationEx' function, passing in the following parameters:
    - A `const char*` filter text, used to specify which packets to capture and apply the simulation to, you can find the filter syntax on the WinDivert documentation.
    - A pointer to the `Config` struct you created before.
    - The number of configs.
    - A `HANDLE`event, which will be used to notify when the initialization of the simulation is done.

  In C:
  ```js
  #include "NetSimulatorAPI.h"
  #include "pipe_reader.h"

  DWORD WINAPI startSimulationThread(LPVOID lpParam)
  {
      // Import the necessary functions
      __declspec(dllimport) void startSimulation(const char* filterText, HANDLE eventHandle);

      __declspec(dllimport) void startSimulationEx(const char* filterText,
      Config* configs, int configsCount, HANDLE eventHandle);

      // Cast the LPVOID argument back to an array of void pointers
      void** args = (void**)lpParam;

      // Extract arguments from the array
      const char* filter = (const char*)args[0];
      HANDLE eventHandle = (HANDLE)args[1];
      Config* configs = (Config*)args[2];
      int numConfigs = *(int*)args[3];

      // Start the simulation using the arguments
      startSimulationEx(filter, configs, numConfigs, eventHandle);

      return 0;
  }

  int main()
  {
      // Import the necessary functions
      __declspec(dllimport) HANDLE createEvent();
      __declspec(dllimport) void waitEvent(HANDLE event);
      __declspec(dllimport) void stopSimulation();

      // Create an event handle to be used in the simulation
      HANDLE eventHandle = createEvent();

      // Configure parameters to pass
      const char* filter = "ip.DstAddr == 140.82.113.4";

      // Create an array of configs
      Config configs[1];
      configs[0] = {true, true, true, 0, 1000, 0, false, SimulationType::Lag};
      int numConfigs = 1;

      // Create an array of void pointers
      void* args[4];

      // Assign the addresses of the filter and eventHandle to the elements of the array
      args[0] = (void*)filter;
      args[1] = eventHandle;    
      args[2] = (void*)configs;
      args[3] = (int*)&numConfigs;

      // Create an start thread to handle the startSimulationThread function and run inmediately 
      HANDLE threadHandle = CreateThread(NULL, 0, startSimulationThread, args, 0, NULL);

      // Wait for the event (initialization of the simulation)
      waitEvent(eventHandle);

      // Continue with your code
      ...

      // Close the threads before exit
      CloseHandle(threadHandle);

      return 0;
  }
  ```

  In C++:
  ```js
  #include "NetSimulatorAPI.h"
  #include "pipe_reader.h"
  #include <thread>

  int main()
  {
      // Import the necessary functions
      __declspec(dllimport) HANDLE createEvent();
      __declspec(dllimport) void waitEvent(HANDLE event);
      __declspec(dllimport) void enableLag();
      __declspec(dllimport) void setLagDelay(float delay);
      __declspec(dllimport) void stopSimulation();
      __declspec(dllimport) void startSimulationEx(const char* filterText,
      Config* configs, int configsCount, HANDLE eventHandle);

      Config configs[1];
      configs[0] = { true, true, true, 0, 1000, 0, false, SimulationType::Lag };
      int numConfigs = 1;
      
      // Create and start the thread for startSimulationEx
      std::thread startThread(startSimulationEx, "ip.DstAddr == 140.82.113.4", configs, numConfigs, eventHandle);
      startThread.detach();

      // Wait for the event (initialization of the simulation)
      waitEvent(eventHandle);

      // Continue with your code
      ...
      
      return 0;
  }
  ```

  In C#:
  ```
  static void Main(string[] args)
  {
      ...

      IntPtr eventHandle = createEvent();
      new Thread(() => startSimulationEx("140.82.114.3", configArray, configArray.Length, eventHandle));
      waitEvent(eventHandle);

      // Continue with your code
  }
  ```


5. **Stopping the simulation:**

In C/C++/C# and other languages:
```
stopProgram();
```



















## API Reference
### Enumerations
#### SimulationType
This enumeration lists all the possible types of network conditions that can be simulated.
- `Lag`
- `Drop`
- `Throttle`
- `Duplicate`
- `Shuffle`
- `Shuffle`
- `Encrypt`
- `TrafficShaper`
### Structs
#### Config
This struct is used to configure the network simulation.
| Member   | Type           | Description                                                                                                            |
|----------|----------------|------------------------------------------------------------------------------------------------------------------------|
| enabled  | bool           | Indicates if the simulation is enabled or not.                                                                         |
| inbound  | bool           | Indicates if the simulation should apply to inbound traffic.                                                           |
| outbound | bol            | Indicates if the simulation should apply to outbound traffic.                                                          |
| chance   | float          | The chance that the simulation will be applied, from 0.0 to 1.0.                                                       |
| delay    | float          | The delay in seconds for the lag and throttle simulations.                                                             |
| num      | float          | The number of copies for duplicate, the minimum number of packets for shuffle, and the speed limit for traffic shaper. |
| drop     | bool           | Indicates if the throttle simulation should drop packets.                                                              |
| type     | SimulationType | The type of simulation to apply.                                                                                       |

### Functions
#### startSimulationEx
```
EXPORT void startSimulationEx(const char* filterText, Config* configs, int configsCount, HANDLE eventHandle);
```
This function starts the simulation of the specified network conditions,
as defined in the array of `Config` structures passed as a parameter.
The `filterText` parameter is a string specifying which network traffic
should be affected by the simulation. The `configsCount` parameter indicates
the number of elements in the `configs` array. The `eventHandle` parameter
is a handle to an event that will be signaled when the simulation has started.
#### startSimulation
```
EXPORT void startSimulation(const char* filterText, HANDLE eventHandle);
```
This function starts the simulation with the specified filter text and event handle.
#### stopSimulation
```
EXPORT void stopSimulation();
```
This function stops the current simulation.
#### SetLag
```
EXPORT void SetLag(bool enabled, bool inbound, bool outbound, float delay);
```
This function sets the lag module with the specified parameters.
#### SetDrop
```
EXPORT void SetDrop(bool enabled, bool inbound, bool outbound, float chance);
```
This function sets the drop module with the specified parameters.
#### SetDuplicate
```
EXPORT void SetDuplicate(bool enabled, bool inbound, bool outbound, float chance, unsigned int numCopies);
```
This function sets the duplicate module with the specified parameters.
#### SetShuffle
```
EXPORT void SetShuffle(bool enabled, bool inbound, bool outbound, float chance, unsigned int minPackets);
```
This function sets the shuffle module with the specified parameters.
#### SetTrafficShaper
```
EXPORT void SetTrafficShaper(bool enabled, bool inbound, bool outbound, float chance, float speedLimit);
```
This function sets the traffic shaper module with the specified parameters.
#### SetEncrypt
```
EXPORT void SetEncrypt(bool enabled, bool inbound, bool outbound, float chance);
```
This function sets the encrypt module with the specified parameters.
#### SetThrottle
```
EXPORT void SetThrottle(bool enabled, bool inbound, bool outbound, float chance, float delay, bool drop);
```
This function sets the throttle module with the specified parameters.

#### enableLag
```
EXPORT void enableLag();
```
This function enables the lag simulation.
#### disableLag
```
EXPORT void disableLag();
```
This function disables the lag simulation.
#### isLagEnabled
```
EXPORT bool isLagEnabled();
```
This function returns a boolean indicating whether the lag simulation is currently enabled or not.
#### setLagDelay
```
EXPORT void setLagDelay(float delay);
```
This function sets the lag delay for the simulation.
#### getLagDelay
```
EXPORT float getLagDelay();
```
The function returns the current lag delay for the network simulation. 
The delay is a float value representing the time in milliseconds that
the function will add a delay to the network packets.
#### setLagInbound
```
EXPORT void setLagInbound(bool inbound);
```
Enables or disables inbound traffic for the lag simulation.
#### getLagInbound
```
EXPORT bool getLagInbound();
```
This function returns a boolean indicating whether the lag inbound traffic is currently enabled or not.
#### setLagOutbound
```
EXPORT void setLagOutbound(bool outbound);
```
Enables or disables outbound traffic for the lag simulation.
#### getLagOutbound
```
EXPORT bool getLagOutbound();
```
This function returns a boolean indicating whether the lag outbound traffic is currently enabled or not.

## Usage/Examples

### C example:

```js

#include "NetSimulatorAPI.h"
#include <Windows.h>

DWORD WINAPI startSimulationThread(LPVOID lpParam)
{
    __declspec(dllimport) void startSimulation(const char* filterText, HANDLE eventHandle);
    // Cast the LPVOID argument back to an array of void pointers
    void** args = (void**)lpParam;

    // Extract the filter and eventHandle from the array
    const char* filter = (const char*)args[0];
    HANDLE eventHandle = (HANDLE)args[1];

    // Start the simulation using the filter and eventHandle
    startSimulation(filter, eventHandle);

    return 0;
}

int main()
{
    __declspec(dllimport) HANDLE startPipeClient();
    __declspec(dllimport) void stopPipeClient(HANDLE processHandle);
    __declspec(dllimport) void SetDrop(bool enabled, bool inbound, bool outbound, float chance);
    __declspec(dllimport) void SetDuplicate(bool enabled, bool inbound, bool outbound, float chance, unsigned int numCopies);
    __declspec(dllimport) void SetTrafficShaper(bool enabled, bool inbound, bool outbound, float chance, float speedLimit);
    __declspec(dllimport) void stopSimulation();
    __declspec(dllimport) HANDLE createEvent();
    __declspec(dllimport) void waitEvent(HANDLE eventHandle);

    // Start the pipe client (SlipClient.exe)
    HANDLE pipeClient = startPipeClient();

    // Create an event handle to be used in the simulation
    HANDLE eventHandle = createEvent();
    // Specify the filter
    const char* filter = "true"; // all traffic
    
    // Create thread and run inmediately
    HANDLE threadHandle = CreateThread(NULL, 0, startSimulationThread, args, 0, NULL);
    
    // Wait for the simulation to start
    waitEvent(eventHandle);

    // Configure modules
    SetDrop(true, true, true, 88.42);
    SetDuplicate(true, true, true, 70.35, 14);
    SetTrafficShaper(true, true, true, 60.65, 5);

    // After using the simulation
    stopSimulation();
    stopPipeClient(pipeClient);    
}
```

The following code demonstrates how to create a new thread to start a simulation and configure modules for it.

- It starts by defining a function `DWORD WINAPI startSimulationThread(LPVOID lpParam)`
  which is going to be the entry point for the new thread. This function takes a
  single argument of type LPVOID, which is a void pointer that can be used to pass any
  data to the thread.
- Inside the thread function, it imports the `startSimulation()` function from the library and casts the
  LPVOID argument back to an array of void pointers.
- Then it extracts the filter and eventHandle from the array, and uses them as arguments to call the
  `startSimulation()` function.
- In the main() function, it imports several functions from the library and starts a pipe client (SlipClient.exe).
- Then it creates an event handle to be used in the simulation, specify the filter, and creates a new thread using
  the CreateThread function and passing the thread function startSimulationThread as an argument, as well as an array
  of void pointers that contains the filter and event handle.
- Then it waits for the simulation to start by calling `waitEvent(eventHandle);`
- After that, it configures the modules for the simulation by calling `SetDrop()`, `SetDuplicate()`, and `SetTrafficShaper()`
  functions and passing the required arguments.
- Finally, it stops the simulation and the pipe client by calling `stopSimulation()` and `stopPipeClient(pipeClient)`
  respectively.

All these steps are done to make the simulation works, the new thread is created to run the simulation and the eventHandle
is used to wait for the simulation to start, the filter is used to specify which traffic should be affected by the
simulation, then the modules are configured and the simulation is stopped.















