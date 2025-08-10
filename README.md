# ArduinoLua
Bibliotecas de para embedding de Lua em Arduino Framework

Esta biblioteca permite-lhe incorporar o interpretador Lua diretamente nos seus projetos Arduino, permitindo-lhe executar scripts Lua para controlar o hardware, gerir a lógica de aplicação e criar interfaces dinâmicas.

# Funcionalidades
- **`Integração nativa`**: Executa scripts Lua diretamente no seu microcontrolador.
- **`Extensível`**: Crie e integre facilmente as suas próprias classes C++ para que possam ser usadas a partir de scripts Lua.

# Instalação
Instalação Manual:
1. Faça o **download** do repositório como um **ficheiro ZIP**.
2. No Arduino IDE, vá a `Sketch > Incluir Biblioteca > Adicionar Biblioteca .ZIP....`
3. Selecione o ficheiro ZIP que descarregou.

# Como Começar
Para começar, pode encontrar exemplos detalhados na pasta `examples/.` O exemplo básico **`blink`** mostra como executar um script Lua simples.

# API Reference
Funções e Classes Principais:
- **`LuaEmbed`**: A classe principal para gerir o estado Lua.
  - **`LuaEmbed()`**: Construtor padrão. Prepara o objeto, mas não inicializa o interpretador Lua.
  - **`~LuaEmbed()`**: Destrutor. Limpa o estado Lua (lua_close) e liberta todos os recursos.
  - **`void addLib(const String &name, LuaLibInitFunc libInit)`**: Regista uma biblioteca C++ para ser carregada no ambiente Lua. name é o nome do módulo em Lua (ex: "MyModule"), e libInit é a função de abertura (luaopen_MyModule).
  - **`bool begin(const String &scriptPath, ...)`**: Inicializa o interpretador Lua e executa um script a partir de um ficheiro (ex: no LittleFS). Retorna true em caso de sucesso.
  - **`bool begin_from_script(const String &code, ...)`**: Inicializa o interpretador Lua e executa um script a partir de uma string de código. Retorna true em caso de sucesso.
  - **`void loop()`**: Função Essencial. Deve ser chamada repetidamente no loop() do Arduino. Responsável pela recolha de lixo automática e pela execução de tarefas dentro de uma função opcional **`function loop()`** dentro do script de lua.
  - **`void reportError()`**: Imprime a última mensagem de erro Lua no Monitor Serial.
  - **`void restart()`**: Fecha o estado Lua atual e cria um novo, reiniciando o ambiente completamente.
  - **`lua_State* getLuaState()`**: Retorna o ponteiro para o estado Lua, permitindo interações diretas para uso avançado.
  - **`void commandLine()`**: Inicia uma linha de comandos Lua através do Monitor Serial, útil para depuração.
  - **`void forceGC()`**: Força a execução imediata da recolha de lixo do Lua.
  - **`void runScript(const String &code)`**
    
- **`LuaModuleHelper`**: Ferramentas auxiliares para simplificar a criação de módulos C++ para Lua.
  - **`newObjectWithMeta()`**: Cria um novo objeto userdata com a metatabela especificada e inicializa-o com placement new.
  - **`registerMetatable()`**: Regista uma metatabela com métodos e funções especiais (incluindo __gc).
  - **`gc()`**: Função de garbage collection que chama explicitamente o destrutor do objeto armazenado em userdata.
 
# Porque Usar ArduinoLua?
- **Atualização dinâmica de lógica sem recompilar o firmware**: Mude comportamentos no dispositivo enviando apenas scripts Lua.
- **Integração de lógica complexa com scripts**: Separe lógica de alto nível da implementação de baixo nível em C++.
- **Desenvolvimento rápido e testes interativos no hardware**: Ajuste e teste código diretamente no dispositivo via porta serial.
- **Redução de ciclos de compilação durante prototipagem**: Menos tempo perdido a compilar e carregar firmware.

# Requisitos
- Arduino Framework (ESP32, ESP8266, ou equivalente com memória suficiente)
- Suporte a LittleFS (para carregar scripts a partir de ficheiros)
- IDE Arduino ou PlatformIO
- Para o exemplo MQTT é necessario as bibliotecas https://github.com/Jo4096/MQTT_Arduino_Agent e opcionalmente para testar caso não haja uma segunda esp https://github.com/Jo4096/MQTT_Arduino_Agent

# Modules
Um module é uma ponte entre C/C++ com Lua. Os modules são simples de se integrar, ver por exemplo **`Arduino_lua.hpp`** e **`Arduino_lua.cpp`**. A presença de classes globais de uso unico como por exemplo Serial, ou Wire simplifica muito a criação desses modules pois no fundo a sua implementação vai ser sempre nos seguintes formatos:
1. Obter parametros de Lua
2. Chamar a função
3. dar push de valores (opcional)
4. Retornar 0 ou 1 no l_func_atual para dizer se foi enviado um valor ou não

Modules para classes que podem existir multiplas instancia tipo de <Servo.h>, é necessario o uso das funções do namespace LuaModuleHelper, pois elas envolvem a criação de pointeres e inicialização de metatables. uma vez que Lua não tem classes, apenas tables, que são a junção de arrays + dicts + lists. ver exemplo **`ClassModulesExample`**

# FAQ / Dicas
- **Posso usar OTA para atualizar apenas scripts Lua?** → Sim, basta enviar o ficheiro .lua e reiniciar o interpretador, caso queiras uma versão mais CLI like não reinicies o intrepretador e corre o codigo recebido com luaEmbed.runScript(const String &code, ...).
- **O Lua é rápido o suficiente no ESP32?** → Para lógica de alto nível, sim. Processamento pesado deve ser feito em C++ por exemplo atraves de modules
- **Posso chamar delete no __gc?** → Apenas se usaste new para criar o objeto. Se usaste placement new, chama apenas o destrutor explicitamente. Usa o namespace para ajudar nisso. Ele usa o metodo de placement new e chama o destructor por ti.
- **Preciso libertar manualmente strings e arrays usados pelo Lua?** → Não, o garbage collector trata disso, exceto se tiveres alocado memória manualmente em C++.
- **O que acontece se não registar um __gc?** → Objetos C++ podem não libertar recursos, causando leaks de memória ou de hardware

# Licença e Uso
- Esta biblioteca é criada por mim, como hobby, sem fins lucrativos.
- Podes usar, copiar, modificar e distribuir livremente, sem preocupações legais.
- Se fizeres melhorias ou correções, ficaria muito grato se me informasses, para que eu possa manter o projeto atualizado para todos.
- Não dou garantias nem responsabilizo-me por qualquer uso do código *(portanto, se uma nave espacial explodir por usares este código e ele não estiver incrivelmente otimizado, a culpa é tua, não minha, haha...)*
