
#include "TestModule_Lua.hpp"

//-----------------------------------------------------------------------------
// Implementação das Funções Lua
//-----------------------------------------------------------------------------

// Função 'new' para criar novas instâncias da classe SimpleClass a partir do Lua.
// Retorna um objeto do tipo 'userdata' na stack do Lua.
static int l_printar_new(lua_State *L)
{
    // Cria o objeto C++ no ambiente Lua, já com a metatabela associada
    // para que os métodos possam ser chamados.
    // "SimpleClassMeta" é o nome que a metatabela terá em Lua.
    // O owner do pointer criado é o Lua, não nós. o LuaHelper::newObjectWithMeta usa um placement new para inicializar esse pointer com os parametros dados
    LuaHelper::newObjectWithMeta<SimpleClass>(L, "SimpleClassMeta");
    return 1; // Retorna um valor (o userdata) na stack do Lua.
}

// Função para definir a string interna do objeto C++.
static int l_printar_set(lua_State *L)
{
    // Verifica se o primeiro argumento na stack é um userdata válido
    // do tipo "SimpleClassMeta" e converte-o para um ponteiro C++.
    SimpleClass *obj = static_cast<SimpleClass *>(luaL_checkudata(L, 1, "SimpleClassMeta"));

    // Verifica se o segundo argumento é uma string e converte-o. isto porque obj:set("string") é intrepertado como set(obj, "string") ou seja o primeiro arg é um self 
    const char *nome = luaL_checkstring(L, 2);
    // Chama o método 'set' do objeto C++.
    obj->set(nome);
    return 0; // Não retorna nada para o Lua.
}


// Função para chamar o método 'print' do objeto.
static int l_printar_print(lua_State *L)
{
    // Obtém o ponteiro para a instância C++ a partir do userdata.
    SimpleClass *obj = static_cast<SimpleClass *>(luaL_checkudata(L, 1, "SimpleClassMeta"));
    
    // Chama o método 'print' do objeto C++.
    obj->print();
    return 0; // Não retorna nada.
}


// Função para chamar o método 'print_ao_contrario' do objeto.
static int l_printar_print_ao_contrario(lua_State *L)
{
    // Obtém o ponteiro para a instância C++ a partir do userdata.
    SimpleClass *obj = static_cast<SimpleClass *>(luaL_checkudata(L, 1, "SimpleClassMeta"));

    // Chama o método 'print_ao_contrario' do objeto C++.
    obj->print_ao_contrario();
    return 0; // Não retorna nada.
}


// Função de 'garbage collection' (recolha de lixo) para o objeto C++.
// É chamada automaticamente pelo Lua quando o objeto não é mais referenciado.
// A classe tenha um destructor é boa ideia chamar-lo aqui com obj->~(), o que é exatamente o que o LuaHelper::gc faz.
static int l_printar_gc(lua_State *L)
{
    return LuaHelper::gc<SimpleClass>(L);
}


//-----------------------------------------------------------------------------
// Tabela de Registo
//-----------------------------------------------------------------------------

// Registo de métodos para a metatabela.
// Esta tabela mapeia os nomes de métodos em Lua (ex: "set") para as funções C++
// correspondentes (ex: l_printar_set).
static const luaL_Reg printar_methods[] = {
    {"set", l_printar_set},
    {"print", l_printar_print},
    {"print_ao_contrario", l_printar_print_ao_contrario},
    {nullptr, nullptr}
};




//-----------------------------------------------------------------------------
// Função de Abertura do Módulo
//-----------------------------------------------------------------------------

// Esta é a função principal que Lua chama quando o módulo é carregado.
extern "C" int luaopen_SimpleClass(lua_State *L)
{
    // 1. Registra a metatabela da classe.
    // Associa o nome "SimpleClassMeta" à nossa tabela de métodos e à função
    // de garbage collection. Esta metatabela define o comportamento do objeto
    // quando os métodos são chamados.
    LuaHelper::registerMetatable(L, "SimpleClassMeta", printar_methods, l_printar_gc);

    // 2. Cria uma tabela para o módulo.
    // Esta tabela servirá como o módulo Lua. Ex: `local obj = SimpleClass.new()`
    lua_newtable(L);

    // 3. Adiciona a função 'new' à tabela do módulo.
    // Empurra a função C++ `l_printar_new` para a stack.
    lua_pushcfunction(L, l_printar_new);
    // Associa a função ao nome "new" na tabela, criando o método `SimpleClass.new()`.
    lua_setfield(L, -2, "new");

    // 4. Retorna a tabela do módulo.
    // Deixa a tabela `SimpleClass` no topo da stack. O Lua irá guardá-la no ambiente
    // global sob o nome do módulo.
    return 1;
}
