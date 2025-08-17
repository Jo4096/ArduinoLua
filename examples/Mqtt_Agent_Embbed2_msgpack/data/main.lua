arduino.pinMode(2, "OUTPUT")

function on_message_callback(topic, payload, from)
    print("[RESPOSTA] de " .. from .. ": " .. payload)
end

function ligar_luz(from, topic, payload)
    local sender_id = payload.id or "unknown"
    local message = payload.msg or "sem mensagem"

    print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> RECEBIDO comando 'ligar_luz' de '" .. sender_id .. "'.")
    print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> Mensagem: " .. message)
    
    arduino.digitalWrite(2, 1)
end

function ler_temperatura(from, topic, payload)
    local sender_id = payload.id or "unknown"
    local message = payload.msg or "sem mensagem"

    print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> RECEBIDO comando 'ler_temp' de '" .. sender_id .. "'.")
    print("[Agente Lua: " .. mqtt.getDeviceId() .. "] -> A enviar a temperatura de volta para '" .. sender_id .. "'...")
    local resposta_str = '{"valor": 23.5, "unidade": "C"}'
    mqtt.publishToDeviceFormatted(sender_id, "temp_repo", resposta_str)
end


function pong(from, topic, payload)
    local sender_id = payload.id or "unknown"
    local message = payload.msg or "sem mensagem";
    print("[CMD] 'pong' recebido de "..sender_id);
end

mqtt.addSubscriptionTopic("devices/all/data");
mqtt.addSubscriptionTopic("devices/esp32_quarto/cmd");
mqtt.registerCommand("ligar_luz", ligar_luz)
mqtt.registerCommand("ler_temp", ler_temperatura)
mqtt.registerCommand("pong", pong)

local result = mqtt.begin(true, true)

if not result then
    print("Não se conectou")
end

function loop()
    mqtt.loop()
end