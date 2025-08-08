#include <Arduino.h>
#include "ArduinoLua.hpp"

LuaEmbed lua;

void setup() {
    Serial.begin(115200);
    while(!Serial){}

    lua.addLib("arduino", luaopen_arduino);
    //se matematica é tão bom porque é que não existe matematica 2?
    lua.addLib("math", luaopen_math2); //math.sqrt(numero) math.atan2(y, x) etc...
    //nvm
    Serial.println("ArduinoLua Console:\n>>");
    lua.begin_from_script("");
}

void loop() {
    lua.commandLine(); //vai ler inputs do Serial. cada input é uma linha de codigo de lua. genero cmd
    delay(1);
}