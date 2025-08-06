#include "ArduinoLua.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// STD FUNCTIONS

///////////////////////
// ARDUINO FUNCTIONS //
///////////////////////

// Função para digitalWrite()
// Sintaxe Lua: arduino.digitalWrite(pin, value)
static int l_digitalWrite(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);   // Lê o primeiro argumento (pino)
    int value = luaL_checkinteger(L, 2); // Lê o segundo argumento (valor)
    digitalWrite(pin, value);            // Chama a função Arduino
    return 0;                            // Não retorna nenhum valor para o Lua
}

// Função para digitalRead()
// Sintaxe Lua: value = arduino.digitalRead(pin)
static int l_digitalRead(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int value = digitalRead(pin);
    lua_pushinteger(L, value); // Coloca o resultado na pilha para retornar ao Lua
    return 1;                  // Retorna 1 valor para o Lua
}

static int l_analogWrite(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int value = luaL_checkinteger(L, 2);
    analogWrite(pin, value);
    return 0;
}

static int l_analogRead(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int value = analogRead(pin); // Chama analogRead e guarda o resultado em 'value'
    lua_pushinteger(L, value);   // Coloca 'value' na pilha Lua
    return 1;                    // Retorna 1 valor
}

static int l_pinMode(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    const char *mode_str = luaL_checkstring(L, 2);
    int mode;

    if (strcmp(mode_str, "INPUT") == 0)
    {
        mode = INPUT;
    }
    else if (strcmp(mode_str, "OUTPUT") == 0)
    {
        mode = OUTPUT;
    }
    else if (strcmp(mode_str, "INPUT_PULLUP") == 0)
    {
        mode = INPUT_PULLUP;
    }
    else
    {
        return luaL_error(L, "Invalid pin mode");
    }

    pinMode(pin, mode);
    return 0;
}

static int l_serial_print(lua_State *L)
{
    const char *message = luaL_checkstring(L, 1);
    Serial.print(message);
    return 0;
}

static int l_serial_println(lua_State *L)
{
    const char *message = luaL_checkstring(L, 1);
    Serial.println(message);
    return 0; // Não retorna nada
}

static int l_delay(lua_State *L)
{
    int value = luaL_checkinteger(L, 1);
    delay(value);
    return 0;
}

static int l_millis(lua_State *L)
{
    long value = millis();
    lua_pushinteger(L, value);
    return 1;
}

static int l_micros(lua_State *L)
{
    long value = micros();
    lua_pushinteger(L, value);
    return 1;
}

static int l_delayMicroseconds(lua_State *L)
{
    int value = luaL_checkinteger(L, 1);
    delayMicroseconds(value);
    return 0;
}

static int l_map(lua_State *L)
{
    // Verifica e obtém os 5 argumentos da pilha do Lua
    long value = luaL_checkinteger(L, 1);
    long fromLow = luaL_checkinteger(L, 2);
    long fromHigh = luaL_checkinteger(L, 3);
    long toLow = luaL_checkinteger(L, 4);
    long toHigh = luaL_checkinteger(L, 5);
    long result = (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
    lua_pushinteger(L, result);
    return 1;
}

static int l_constrain(lua_State *L)
{
    long value = luaL_checkinteger(L, 1);
    long min = luaL_checkinteger(L, 2);
    long max = luaL_checkinteger(L, 3);
    long temp_min = (min < max) ? min : max;
    long temp_max = (min < max) ? max : min;
    long result = (value < temp_min) ? temp_min : ((value > temp_max) ? temp_max : value);
    lua_pushinteger(L, result);
    return 1;
}

static int l_pulseIn(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    const char *value_str = luaL_checkstring(L, 2);
    int value_enum;
    if (strcmp(value_str, "HIGH") == 0)
    {
        value_enum = HIGH;
    }
    else if (strcmp(value_str, "LOW") == 0)
    {
        value_enum = LOW;
    }
    else
    {
        return luaL_error(L, "Invalid value for pulseIn. Expected 'HIGH' or 'LOW'.");
    }

    unsigned long duration;
    if (lua_isnoneornil(L, 3))
    {
        duration = pulseIn(pin, value_enum);
    }
    else
    {
        unsigned long timeout = luaL_checkinteger(L, 3);
        duration = pulseIn(pin, value_enum, timeout);
    }
    lua_pushinteger(L, duration);
    return 1;
}

static int l_serial_begin(lua_State *L)
{
    const char *serial_port_name = luaL_checkstring(L, 1);
    long baud_rate = luaL_checkinteger(L, 2);
    if (strcmp(serial_port_name, "Serial") == 0)
    {
        Serial.begin(baud_rate);
    }
    else if (strcmp(serial_port_name, "Serial1") == 0)
    {
        Serial1.begin(baud_rate);
    }
    else if (strcmp(serial_port_name, "Serial2") == 0)
    {
        Serial2.begin(baud_rate);
    }
    else
    {
        return luaL_error(L, "Invalid serial port name. Expected 'Serial', 'Serial1', or 'Serial2'.");
    }

    return 0;
}

static int l_serial_available(lua_State *L)
{
    const char *serial_port_name = luaL_checkstring(L, 1);
    int available_bytes = 0;

    if (strcmp(serial_port_name, "Serial") == 0)
    {
        available_bytes = Serial.available();
    }
    else if (strcmp(serial_port_name, "Serial1") == 0)
    {
        available_bytes = Serial1.available();
    }
    else if (strcmp(serial_port_name, "Serial2") == 0)
    {
        available_bytes = Serial2.available();
    }
    else
    {
        return luaL_error(L, "Invalid serial port name. Expected 'Serial', 'Serial1', or 'Serial2'.");
    }

    lua_pushinteger(L, available_bytes);
    return 1;
}

static int l_serial_read(lua_State *L)
{
    const char *serial_port_name = luaL_checkstring(L, 1);
    int read_byte = -1;

    if (strcmp(serial_port_name, "Serial") == 0)
    {
        read_byte = Serial.read();
    }
    else if (strcmp(serial_port_name, "Serial1") == 0)
    {
        read_byte = Serial1.read();
    }
    else if (strcmp(serial_port_name, "Serial2") == 0)
    {
        read_byte = Serial2.read();
    }
    else
    {
        return luaL_error(L, "Invalid serial port name. Expected 'Serial', 'Serial1', or 'Serial2'.");
    }
    lua_pushinteger(L, read_byte);
    return 1;
}

static int l_serial_read_string(lua_State *L) {
    const char *serial_port_name = luaL_checkstring(L, 1);
    String received_string = "";
    int read_byte = -1;

    // Acede ao objeto Serial correto
    HardwareSerial* serial_port = nullptr;
    if (strcmp(serial_port_name, "Serial") == 0) {
        serial_port = &Serial;
    } else if (strcmp(serial_port_name, "Serial1") == 0) {
        serial_port = &Serial1;
    } else if (strcmp(serial_port_name, "Serial2") == 0) {
        serial_port = &Serial2;
    } else {
        return luaL_error(L, "Invalid serial port name. Expected 'Serial', 'Serial1', or 'Serial2'.");
    }

    // Lê até o fim de linha ou até não haver mais dados
    while (serial_port->available()) {
        read_byte = serial_port->read();
        if (read_byte == -1) {
            break;
        }
        received_string += (char)read_byte;

        // Se encontrar um final de linha, parar de ler
        if (read_byte == '\n') {
            break;
        }
    }

    lua_pushstring(L, received_string.c_str());
    return 1;
}

static int l_tone(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int frequency = luaL_checkinteger(L, 2);
    int duration = luaL_optinteger(L, 3, 0);
    if (duration > 0)
        tone(pin, frequency, duration);
    else
        tone(pin, frequency);
    return 0;
}

static int l_noTone(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    noTone(pin);
    return 0;
}

////////////////////////
//      Wire.h        //
////////////////////////

static int l_i2cBegin(lua_State *L)
{
    int sda = luaL_checkinteger(L, 1);
    int scl = luaL_checkinteger(L, 2);
    Wire.begin(sda, scl);
    return 0;
}

static int l_i2cBeginTransmission(lua_State *L)
{
    int address = luaL_checkinteger(L, 1);
    Wire.beginTransmission(address);
    return 0;
}

static int l_i2cWrite(lua_State *L)
{
    const char *data = luaL_checkstring(L, 1);
    Wire.write((const uint8_t *)data, strlen(data));
    return 0;
}

static int l_i2cEndTransmission(lua_State *L)
{
    int stop = luaL_optinteger(L, 1, true); // default true
    int result = Wire.endTransmission(stop);
    lua_pushinteger(L, result);
    return 1;
}

static int l_i2cRequestFrom(lua_State *L)
{
    int address = luaL_checkinteger(L, 1);
    int quantity = luaL_checkinteger(L, 2);
    Wire.requestFrom(address, quantity);
    return 0;
}

static int l_i2cAvailable(lua_State *L)
{
    int available = Wire.available();
    lua_pushinteger(L, available);
    return 1;
}

static int l_i2cRead(lua_State *L)
{
    int byte = Wire.read();
    lua_pushinteger(L, byte);
    return 1;
}

///////////////////////////////
//          SPI.h            //
///////////////////////////////

static int l_spiBegin(lua_State *L)
{
    int sck = luaL_checkinteger(L, 1);
    int miso = luaL_checkinteger(L, 2);
    int mosi = luaL_checkinteger(L, 3);
    int ss = luaL_checkinteger(L, 4);
    SPI.begin(sck, miso, mosi, ss);
    return 0;
}

static int l_spiTransfer(lua_State *L)
{
    int data = luaL_checkinteger(L, 1);
    uint8_t result = SPI.transfer(data);
    lua_pushinteger(L, result);
    return 1;
}

static const struct luaL_Reg arduino_lib[] = {
    // Arduino
    {"digitalWrite", l_digitalWrite},
    {"digitalRead", l_digitalRead},
    {"analogWrite", l_analogWrite},
    {"analogRead", l_analogRead},
    {"pinMode", l_pinMode},
    {"millis", l_millis},
    {"micros", l_micros},
    {"delay", l_delay},
    {"delayMicroseconds", l_delayMicroseconds},
    {"map", l_map},
    {"constrain", l_constrain},
    {"pulseIn", l_pulseIn},
    {"serialBegin", l_serial_begin},
    {"serialPrint", l_serial_print},
    {"serialPrintln", l_serial_println},
    {"serialAvailable", l_serial_available},
    {"serialRead", l_serial_read},
    {"serialReadString", l_serial_read_string},
    {"tone", l_tone},
    {"noTone", l_noTone},
    // I2C
    {"i2cBegin", l_i2cBegin},
    {"i2cBeginTransmission", l_i2cBeginTransmission},
    {"i2cWrite", l_i2cWrite},
    {"i2cEndTransmission", l_i2cEndTransmission},
    {"i2cRequestFrom", l_i2cRequestFrom},
    {"i2cAvailable", l_i2cAvailable},
    {"i2cRead", l_i2cRead},
    // SPI
    {"spiBegin", l_spiBegin},
    {"spiTransfer", l_spiTransfer},
    {NULL, NULL},
};

extern "C" int luaopen_arduino(lua_State *L)
{
    luaL_newlib(L, arduino_lib);
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////