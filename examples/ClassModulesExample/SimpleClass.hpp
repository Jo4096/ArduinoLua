#ifndef MELHOR_CLASSE_DE_SEMPRE_HPP
#define MELHOR_CLASSE_DE_SEMPRE_HPP


//Classe simples

class SimpleClass
{
    private:
    String nome;
    public:
    SimpleClass() : nome("")
    {

    }

    void set(const String nome)
    {
        this->nome = nome;
    }

    void print()
    {
        Serial.println(nome);
    }

    void print_ao_contrario()
    {
        for(int i = nome.length() - 1; i >= 0; i--)
        {
            Serial.print(nome[i]);
        }
        Serial.print("\n");
    } 
    
};

#endif