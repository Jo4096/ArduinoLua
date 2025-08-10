#include <ArduinoLua.hpp>
#include "TestModule_Lua.hpp"


// Declara o objeto LuaEmbed. Esta é a interface principal
// para integrar scripts Lua no seu projeto Arduino.
LuaEmbed lua;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // Adiciona uma biblioteca C++ ao ambiente Lua. A string "SimpleClass"
  // é o nome que a biblioteca terá em Lua. O segundo argumento,
  // luaopen_SimpleClass, é a função C que regista a
  // classe C++ e os seus métodos para que possam ser chamados a partir de Lua.
  lua.addLib("SimpleClass", luaopen_SimpleClass);

  // Define o script Lua como uma string literal. A sintaxe R"( ... )"
  // facilita a escrita de strings de várias linhas sem a necessidade de escapar as aspas.
  // Os comentários foram removidos para otimizar o uso da memória.
  const char* code = R"(
    print("Iniciando o script Lua...")
    local obj1 = SimpleClass.new()
    local obj2 = SimpleClass.new()
    obj1:set("Este e o primeiro objeto, uma otima classe de exemplo!")
    obj2:set("Este e o segundo objeto, provando que sao separados.")
    obj1:print()
    obj1:print_ao_contrario()
    obj2:print()
    obj2:print_ao_contrario()
    print("Script Lua finalizado.")
  )";

  // Inicia o interpretador Lua e executa o script fornecido uma vez.
  lua.begin_from_script(code);
}

void loop() {
  // Isto é necessário para manter o estado Lua e a recolha de lixo a funcionar.
  // sejam processadas e para evitar fugas de memória do estado Lua.
  lua.loop();
}
