# ArduinoLua
Libraries for embedding Lua into the Arduino Framework

This library allows you to embed the Lua interpreter directly into your Arduino projects, enabling you to run Lua scripts to control hardware, manage application logic, and create dynamic interfaces.

# Features
- **`Native integration`**: Runs Lua scripts directly on your microcontroller.
- **`Extensible`**: Easily create and integrate your own C++ classes so they can be used from Lua scripts.

# Installation
Manual Installation:
1. Download the **repository** as a **ZIP file**.
2. In Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library...`
3. Select the ZIP file you downloaded.

# Getting Started
To get started, you can find detailed examples in the `examples/` folder. The basic example **`blink`** shows how to run a simple Lua script.

# API Reference
Main Functions and Classes:
- **`LuaEmbed`**: The main class for managing the Lua state.
  - **`LuaEmbed()`**: Default constructor. Prepares the object but does not initialize the Lua interpreter.
  - **`~LuaEmbed()`**: Destructor. Cleans up the Lua state (lua_close) and frees all resources.
  - **`void addLib(const String &name, LuaLibInitFunc libInit)`**: Registers a C++ library to be loaded into the Lua environment. `name` is the name of the module in Lua (e.g., "MyModule"), and `libInit` is the opening function (luaopen_MyModule).
  - **`bool begin(const String &scriptPath, ...)`**: Initializes the Lua interpreter and runs a script from a file (e.g., in LittleFS). Returns true on success.
  - **`bool begin_from_script(const String &code, ...)`**: Initializes the Lua interpreter and runs a script from a code string. Returns true on success.
  - **`void loop()`**: Essential function. Must be called repeatedly in Arduino's loop(). Responsible for automatic garbage collection and executing tasks within an optional **`function loop()`** inside the Lua script.
  - **`void reportError()`**: Prints the last Lua error message to the Serial Monitor.
  - **`void restart()`**: Closes the current Lua state and creates a new one, completely restarting the environment.
  - **`lua_State* getLuaState()`**: Returns the pointer to the Lua state, allowing direct interactions for advanced use.
  - **`void commandLine()`**: Starts a Lua command line via the Serial Monitor, useful for debugging.
  - **`void forceGC()`**: Forces immediate execution of Lua's garbage collection.
  - **`void runScript(const String &code)`**
    
- **`LuaModuleHelper`**: Helper tools to simplify creating C++ modules for Lua.
  - **`newObjectWithMeta()`**: Creates a new userdata object with the specified metatable and initializes it with placement new.
  - **`registerMetatable()`**: Registers a metatable with methods and special functions (including __gc).
  - **`gc()`**: Garbage collection function that explicitly calls the destructor of the object stored in userdata.
 
# Why Use ArduinoLua?
- **Dynamically update logic without recompiling firmware**: Change device behavior by sending only Lua scripts.
- **Integrate complex logic with scripts**: Separate high-level logic from low-level implementation in C++.
- **Rapid development and interactive testing on hardware**: Adjust and test code directly on the device via serial port.
- **Reduce compile cycles during prototyping**: Less time wasted compiling and uploading firmware.

# Requirements
- Arduino Framework (ESP32, ESP8266, or equivalent with enough memory)
- LittleFS support (to load scripts from files)
- Arduino IDE or PlatformIO
- For the MQTT example, you need the library https://github.com/Jo4096/MQTT_Arduino_Agent and optionally, for testing if there is no second ESP, https://github.com/Jo4096/MQTT_Python_Agent

# Modules
A module is a bridge between C/C++ and Lua. Modules are simple to integrate; see for example **`Arduino_lua.hpp`** and **`Arduino_lua.cpp`**. The presence of global single-use classes such as Serial or Wire greatly simplifies the creation of these modules because their implementation always follows the same pattern:
1. Get parameters from Lua
2. Call the function
3. Push values (optional)
4. Return 0 or 1 in the current l_func to indicate whether a value was sent or not

For classes that may have multiple instances, like those from `<Servo.h>`, it is necessary to use functions from the LuaModuleHelper namespace, as they involve creating pointers and initializing metatables. Lua itself has no classes, only tables, which are a combination of arrays + dicts + lists. See example **`ClassModulesExample`**.

# FAQ / Tips
- **Can I use OTA to update only Lua scripts?** → Yes, just send the .lua file and restart the interpreter; if you want a more CLI-like version, don’t restart the interpreter and run the received code with `luaEmbed.runScript(const String &code, ...)`.
- **Is Lua fast enough on ESP32?** → For high-level logic, yes. Heavy processing should be done in C++, for example through modules.
- **Can I call delete in __gc?** → Only if you used new to create the object. If you used placement new, just call the destructor explicitly. The namespace helps with this. It uses placement new and calls the destructor for you.
- **Do I need to manually free strings and arrays used by Lua?** → No, the garbage collector takes care of that, except if you manually allocated memory in C++.
- **What happens if I don’t register a __gc?** → C++ objects may not free resources, causing memory or hardware leaks.

# License and Usage
- This library is created by me, as a hobby, without commercial purposes.
- You can use, copy, modify, and distribute it freely without legal concerns.
- If you make improvements or fixes, I’d be grateful if you let me know so I can keep the project up to date for everyone.
- I provide no warranties and take no responsibility for any use of the code *(so, if a spaceship explodes because you used this code and it’s not incredibly optimized, it’s your fault, not mine, haha...)*
