#include <stdio.h>

#define NOME_ARQUIVO "exemplo.bin"

int main()
{
    // Declarar ponteiro do arquivo
    FILE *ponteiro_arquivo;

    // 1. Abrir arquivo usando rb = read binary, escrita em binario
    // 2. retorna um ponteiro para o arquivo
    ponteiro_arquivo = fopen(NOME_ARQUIVO, "rb");

    // Checa se abriu corretamente
    if (ponteiro_arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return -1;
    }

    int bytes_lidos;

    // Exemplo 1: Ler um único número no arquivo
    int um_numero;

    // 1. fread = escreve no arquivo
    //  arg1: endereço para salvar o valor lido
    //  arg2: tamanho de cada valor
    //  arg3: quantos valores serão lidos
    //  arg4: ponteiro do arquivo
    // 2. retorna numero de bytes lidos
    bytes_lidos = fread(&um_numero, sizeof(int), 1, ponteiro_arquivo);

    // checar se ele escreveu algo
    if (bytes_lidos == 0)
    {
        printf("Erro ao ler arquivo");
        return -1;
    }

    printf("Exemplo 1 - Valor lido: %d\n", um_numero);

    // Exemplo 2: Ler um caractere
    char um_caractere;
    bytes_lidos = fread(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    if (bytes_lidos == 0)
    {
        printf("Erro ao escrever no arquivo");
        return -1;
    }

    printf("Exemplo 2 - Valor lido: %c\n", um_caractere);

    // Exemplo 3: Lendo múltiplos valores - parte 1
    fread(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    printf("Exemplo 3.0 - Valor lido: %c\n", um_caractere);

    fread(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    printf("Exemplo 3.1 - Valor lido: %c\n", um_caractere);

    fread(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    printf("Exemplo 3.2 - Valor lido: %c\n", um_caractere);

    fread(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    printf("Exemplo 3.3 - Valor lido: %c\n", um_caractere);

    // Exemplo 4: Ler um vetor de valores
    char mensagem[13];

    fread(&mensagem, sizeof(char), 13, ponteiro_arquivo);
    printf("Exemplo 4 - Valor lido: %s\n", mensagem);

    // Exemplo 5: Rebobinando o ponteiro do arquivo

    rewind(ponteiro_arquivo);

    int valor_lido;
    fread(&valor_lido, sizeof(int), 1, ponteiro_arquivo);
    printf("Exemplo 5 - Valor lido: %d\n", valor_lido);

    // Exemplo 6: Lendo um valor "errado"
    int valor_exemplo_6;
    fread(&valor_exemplo_6, sizeof(int), 1, ponteiro_arquivo);
    printf("Exemplo 6 - Valor lido: %d\n", valor_exemplo_6);

    // Exemplo 7: Lendo um arquivo de tamanho indefinido
    char string[30];
    int i = 0;
    while (!feof(ponteiro_arquivo))
    {
        char valor_exemplo_7;
        fread(&valor_exemplo_7, sizeof(char), 1, ponteiro_arquivo);
        printf("Exemplo 7.%d - Valor lido: %c\n", i, valor_exemplo_7);

        string[i] = valor_exemplo_7;

        i++;
    }
    string[i] = '\0';

    printf("Exemplo 7.final - String lida: %s\n", string);

    // Fechar o arquivo
    fclose(ponteiro_arquivo);

    return 0;
}