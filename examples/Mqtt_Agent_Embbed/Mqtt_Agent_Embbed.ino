#include <Arduino.h>
#include "LuaEmbedCJJ.hpp"
#include "ArduinoLua.hpp"
#include "MQTT_Agent_Lua.hpp"

LuaEmbed lua;

void setup() {
    Serial.begin(115200);
    delay(1000);

    lua.addLib("arduino", luaopen_arduino);
    lua.addLib("mqtt", luaopen_MQTT_Agent);

    String code = R"(
        local ssid = "SSID"
        local password = "PASSWORD"
        local mqttserver = "IP"
        local mqttuser = "USERNAME_BROKER"
        local mqttpass = "PASSWORD_BROKER"
        local deviceID = "esp32_quarto"

        arduino.pinMode(2, "OUTPUT")

        function on_message_callback(topic, payload, from)
            print("[RESPOSTA] de " .. from .. ": " .. payload)
        end

        function ligar_luz(from, topic, payload)
            local sender_id = payload.sender_id or "unknown"
            local message = payload.message or "sem mensagem"

            print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> RECEBIDO comando 'ligar_luz' de '" .. sender_id .. "'.")
            print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> Mensagem: " .. message)
            
            arduino.digitalWrite(2, 1)
        end

        function ler_temperatura(from, topic, payload)
            local sender_id = payload.sender_id or "unknown"
            local message = payload.message or "sem mensagem"

            print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> RECEBIDO comando 'ler_temperatura' de '" .. sender_id .. "'.")
            print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> A enviar a temperatura de volta para '" .. sender_id .. "'...")
            local resposta_str = '{"valor": 23.5, "unidade": "C"}'
            mqtt.publishToDeviceFormatted(sender_id, "temperatura_report", resposta_str)
        end


        function pong(from, topic, payload)
            local sender_id = payload.sender_id or "unknown"
            local message = payload.message or "sem mensagem";
            print("[CMD] 'pong' recebido de "..sender_id);
        end

        mqtt.config(ssid, password, mqttserver, mqttuser, mqttpass, deviceID)
        mqtt.addSubscriptionTopic("devices/all/data");
        mqtt.addSubscriptionTopic("devices/esp32_quarto/cmd");
        mqtt.registerCommand("ligar_luz", ligar_luz)
        mqtt.registerCommand("ler_temperatura", ler_temperatura)
        mqtt.registerCommand("pong", pong)
        
        local result = mqtt.begin()

        if not result then
            print("Não se conectou")
        end

        function loop()
            mqtt.loop()
        end
    )";

    //Nota, mqtt em lua é uma referencia directa à variavel global Agent no sentido de que chamar as funções no script chama as funções de C++ da classe (tal como Serial Wire SPI)
    //Isto serve para prevenir que haja crash associados a criação de mais do que uma MQTT_Agent
    //E torna mais flexivel a programação porque podemos configurar o mqtt dentro e fora do script (inclusive registar Commandos, .config() .begin())
    //A unica coisa que é mais complexo por assim dizer é a alocação de func commando dentro do script pois o garbage collector de lua não apanha as functions então usei apenas um map para destruir os pointers criados para um determinado nome

    if (!lua.begin_from_script(code)) {
        Serial.println("Erro ao executar script Lua!");
    }
}

void loop() {
    lua.loop(); //Ou tambem podiam chamar Agent.loop() e não ter function loop(), é a mesma variavel
    delay(1);
}
