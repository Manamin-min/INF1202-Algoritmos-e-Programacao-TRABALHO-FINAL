#include <stdio.h>

#define NOME_ARQUIVO "exemplo.bin"

int main()
{
    // Declarar ponteiro do arquivo
    FILE *ponteiro_arquivo;

    // 1. Abrir arquivo usando wb = write binary, escrita em binario
    // 2. retorna um ponteiro para o arquivo
    ponteiro_arquivo = fopen(NOME_ARQUIVO, "wb");

    // Checa se abriu corretamente
    if (ponteiro_arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return -1;
    }

    int bytes_escritos;

    // Exemplo 1: Escrevendo um único número no arquivo
    int um_numero = 99;

    // 1. fwrite = escreve no arquivo
    //  arg1: endereço dos valores a serem salvos
    //  arg2: tamanho de cada valor
    //  arg3: quantos valores serão escritos
    //  arg4: ponteiro do arquivo
    // 2. retorna numero de bytes escritos
    bytes_escritos = fwrite(&um_numero, sizeof(int), 1, ponteiro_arquivo);

    // checar se ele escreveu algo
    if (bytes_escritos == 0)
    {
        printf("Erro ao escrever no arquivo");
        return -1;
    }

    // Exemplo 2: Escrevendo um caractere
    char um_caractere = 'L';
    bytes_escritos = fwrite(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    if (bytes_escritos == 0)
    {
        printf("Erro ao escrever no arquivo");
        return -1;
    }

    // Exemplo 3: Inserindo múltiplos valores - parte 1
    fwrite(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    fwrite(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    fwrite(&um_caractere, sizeof(char), 1, ponteiro_arquivo);
    fwrite(&um_caractere, sizeof(char), 1, ponteiro_arquivo);

    // Exemplo 4: Inserindo um vetor de valores
    char mensagem[] = "Lissa fedida";

    fwrite(&mensagem, sizeof(char), 13, ponteiro_arquivo);

    // Fechar o arquivo
    fclose(ponteiro_arquivo);

    return 0;
}