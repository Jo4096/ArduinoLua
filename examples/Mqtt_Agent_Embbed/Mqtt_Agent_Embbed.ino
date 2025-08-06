#include <Arduino.h>
#include "LuaEmbedCJJ.hpp"
#include "MQTT_Agent_Lua.hpp"   // Binding Lua para a biblioteca MQTT_Agent (pode ficar sem, mas recomendado)
#include "ArduinoLua.hpp"       // Biblioteca Lua para Arduino (se disponível)


LuaEmbed lua;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Regista as bibliotecas Lua que estarão disponíveis para os scripts Lua:
  // "arduino" expõe funções básicas de Arduino como pinMode, digitalWrite, delay, etc.
  // "MQTT_Agent" expõe a classe MQTT_Agent para comunicação MQTT
  lua.addLib("arduino", luaopen_arduino);
  lua.addLib("MQTT_Agent", luaopen_MQTT_Agent);


  // Código Lua embebido como string
  // Este script Lua cria um agente MQTT, subscreve tópicos, regista comandos, e reage a mensagens
  String code = R"(
    local ssid = "SSID"
    local password = "PASSWORD" 
    local mqttServer = "192.168.1.100" -- IP DO PC/broker MQTT
    local mqttUsername = "user"       -- username do broker MQTT
    local mqttPassword = "pass"       -- password do broker MQTT
    local deviceId = "esp32_quarto"

    -- Configura o pino 2 como saída para controlar o LED
    arduino.pinMode(2, "OUTPUT")

    -- Cria uma instância do agente MQTT com as credenciais fornecidas
    local Agent = MQTT_Agent.new(ssid, password, mqttServer, mqttUsername, mqttPassword, deviceId)

    -- Função de callback para mensagens recebidas (respostas)
    local function onMessage(from, topic, payload)
        print(string.format("[RESPOSTA] de %s: %s", from, payload))
    end

    -- Função para comando "ligar_luz" recebida via MQTT
    local function ligar_luz(from, topic, message)
        print(string.format("[Agente 2: %s] -> RECEBIDO comando 'ligar_luz' de '%s'.", deviceId, from))
        print(string.format("[Agente 2: %s] -> Mensagem: %s", deviceId, message))
        -- Liga o LED físico (pino 2)
        arduino.digitalWrite(2, 1)
    end

    -- Função para comando "ler_temperatura" recebida via MQTT
    local function ler_temperatura(from, topic, message)
        print(string.format("[Agente 2: %s] -> RECEBIDO comando 'ler_temperatura' de '%s'.", deviceId, from))
        print(string.format("[Agente 2: %s] -> A enviar a temperatura de volta para '%s'...", deviceId, from))

        -- Simula resposta com dados de temperatura
        local resposta = {
            valor = 23.5,
            unidade = "C"
        }

        -- Serializa a resposta manualmente em JSON (para simplificar)
        local resposta_json = '{"valor": 23.5, "unidade": "C"}'

        -- Envia a resposta formatada de volta ao remetente
        Agent:publishToDeviceFormatted(from, "temperatura_report", resposta_json)
    end

    -- Subscreve aos tópicos de comandos gerais e específicos deste dispositivo
    Agent:addSubscriptionTopic("devices/all/data")
    Agent:addSubscriptionTopic("devices/" .. deviceId .. "/cmd")

    -- Regista funções para os comandos MQTT recebidos
    Agent:registerCommand("pong", function(from, topic, message)
        print("[DEFAULT PONG RECEBIDO] de: " .. from)
    end)

    Agent:registerCommand("ligar_luz", ligar_luz)
    Agent:registerCommand("ler_temperatura", ler_temperatura)

    -- Define callback para mensagens de resposta (não comandos)
    Agent:setOnMessageCallback(onMessage)

    -- Inicia o agente MQTT com ping ativo
    Agent:begin(true)

    -- Função principal de loop que será chamada continuamente
    function loop()
        Agent:loop()        -- Mantém a ligação MQTT viva e processa mensagens
        arduino.delay(10)   -- Pequeno delay para não sobrecarregar o microcontrolador
    end
  )";


  // Executa o script Lua embebido; se falhar, indica no Serial Monitor
  if (!lua.begin_from_script(code)) {
    Serial.println("Erro ao executar script Lua!");
  }
}

void loop() {
  // Loop principal do Arduino chama o loop do interpretador Lua
  lua.loop();
  delay(1);
}

/*
Comentários Gerais
Este exemplo demonstra a biblioteca MQTT_Agent embebida em Lua para ESP32.

Funciona igual ao exemplo original em C++ (exemplo_simples.ino) mas usando um interpretador Lua embutido.

Permite definir e alterar a lógica MQTT e de controlo diretamente via scripts Lua, aumentando flexibilidade e facilidade de manutenção.

Mostra como registar comandos MQTT que podem ser acionados remotamente, bem como enviar respostas formatadas.

Usa funções do binding Lua arduino para manipular hardware (ex.: pinMode, digitalWrite, delay).

É útil para prototipagem rápida, testes dinâmicos, ou dispositivos IoT programáveis remotamente.

*/
