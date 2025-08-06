#include <Arduino.h>
#include "LuaEmbedCJJ.hpp"   // Tua classe de embed Lua personalizada
#include "ArduinoLua.hpp"    // Biblioteca que expõe funções Arduino para Lua

LuaEmbed lua;  // Instância do motor Lua embedado

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação Serial a 115200 baud
  delay(1000);           // Pequeno delay para garantir que a serial está pronta

  lua.addLib("arduino", luaopen_arduino);  
  // Adiciona a biblioteca "arduino" ao ambiente Lua (funções como pinMode, digitalWrite, delay, millis, serialPrint, serialPrintln, etc...)

  // Script Lua que implementa o Blink
  String code = R"(
  
    local ledPin = 2
    arduino.pinMode(ledPin, "OUTPUT")  -- Configura o pino 2 como saída
    
    local ledState = false  -- Estado atual do LED
    
    function loop()
      ledState = not ledState  -- Alterna o estado (liga/desliga)
      arduino.digitalWrite(ledPin, ledState and 1 or 0)  -- Escreve no pino
      
      if ledState then
        print("LED LIGADO")
      else
        print("LED DESLIGADO")
      end
      
      arduino.delay(500)  -- Espera 500 ms antes de alternar de novo
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
