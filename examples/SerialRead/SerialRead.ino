#include <Arduino.h>
#include "ArduinoLua.hpp"    // Biblioteca que expõe funções Arduino para Lua

LuaEmbed lua;  // Instância do motor Lua embedado

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação Serial a 115200 baud
  delay(1000);           // Pequeno delay para garantir que a serial está pronta

  // Adiciona a biblioteca "arduino" ao ambiente Lua (funções como pinMode, digitalWrite, serialReadString, etc.)
  lua.addLib("arduino", luaopen_arduino);

  // Script Lua que lê comandos da Serial e controla o LED
  String code = R"(
    local ledPin = 2
    arduino.pinMode(ledPin, "OUTPUT")  -- Configura o pino 2 como saída

    function loop()
      local message = arduino.serialReadString("Serial")  -- Lê string da Serial
      
      if message == "ON\n" then  -- Se receber "ON", liga o LED
        arduino.digitalWrite(ledPin, 1)
        print("LED LIGADO")
      elseif message == "OFF\n" then  -- Se receber "OFF", desliga o LED
        arduino.digitalWrite(ledPin, 0)
        print("LED DESLIGADO")
      end
    end
  )";

  // Inicializa o motor Lua e executa o script acima
  if (!lua.begin_from_script(code)) 
  {
    Serial.println("Erro ao executar script Lua!");
  }
}

void loop() 
{
  lua.loop();  // Chama a função loop() do script Lua continuamente
  delay(1);
}
