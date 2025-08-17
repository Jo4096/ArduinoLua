#include <Arduino.h>
#include "ArduinoLua.hpp"
#include "MQTT_Agent_Lua.hpp"

LuaEmbed lua;

void setup() {
    Serial.begin(115200);
    delay(1000);

    if(!LittleFS.begin())
    {
        while(1) {Serial.println("Tamos lixados\n"); delay(1000);}
    }

    lua.addLib("arduino", luaopen_arduino);
    lua.addLib("mqtt", luaopen_MQTT_Agent);

    Agent.config("YOUR_SSID", "YOUR_PASSWORD", "BROKERS_IP", "BROKER_USER_NAME", "BROKERS_PASSWORD", "esp32_quarto");
    Agent.setBufferSize(1024);


    if (!lua.begin("/main.lua")) {
        Serial.println("Erro ao executar script Lua!");
    }
}

void loop() {
    lua.loop();
    delay(1);
}
