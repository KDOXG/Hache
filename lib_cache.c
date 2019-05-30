#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** -------- CACHE --------
 * Funções para se usar na Cache.
 */

struct Cache{
    int tag;
    int policyControl;
    char bit_valid;
};

struct Miss{
    int compulsory;
    int capacity;
    int conflict;
};

typedef struct Endereco{
    int a;
    char nuu;
}Address;

void setReplacement(struct Cache **cache, char policy, int i, int assoc, int index, char flag)
{
    if (policy >> 0 == 0)//default
        return;
    if (policy >> 1 == 0)//LRU
    {
        if (cache[i][index].policyControl == assoc-1)
            return;
        int j, id=cache[i][index].policyControl;
        cache[i][index].policyControl = assoc-1;
        for (j=0; j<assoc; j++)
        {
            if (cache[j][index].bit_valid != 0 && cache[j][index].policyControl > id && j != i)
                cache[j][index].policyControl--;
        }
        return;
    }
    if (policy >> 2 == 0)//LFU
    {
        if (flag == 0b00000001 || flag == 0b00000010)//flags de miss
            cache[i][index].policyControl = 0;
        else
            cache[i][index].policyControl++;
        return;
    }
    if (policy >> 3 == 0)//FIFO
    {
        if (flag == 0b00000000)//flag de hit
        {
            return;
        }
        if (flag == 0b00000010)//flag de miss compulsorio
        {
            int j, pol=-1;
            for (j=0; j<assoc && cache[j][index].bit_valid != 0; j++)
                pol++;
            cache[j-1][index].policyControl = pol;
            return;
        }
        int j, id;
        for (j=0; j<assoc; j++)
            if (cache[j][index].policyControl == 0)
                id = j;
        cache[id][index].policyControl = assoc-1;
        for (j=0; j<assoc; j++)
            if (j != id)
                cache[j][index].policyControl--;
        return;
    }
    if (policy >> 4 == 0)//LIFO
        return;
    return;
}

int getReplacement(struct Cache **cache, char policy, int assoc, int index)
{
    if (policy >> 0 == 0)//default
        return rand() % assoc;
    if (policy >> 1 == 0)//LRU
    {
        int i, id, pol=assoc;
        for (i=0; i<assoc; i++)
        {
            if (cache[i][index].policyControl < pol)
            {
                pol = cache[i][index].policyControl;
                id = i;
            }
        }
        return id;
    }
    if (policy >> 2 == 0)//LFU
    {
        int i, id, pol=cache[0][index].policyControl;
        for (i=0; i<assoc; i++)
        {
            if (cache[i][index].policyControl < pol)
            {
                pol = cache[i][index].policyControl;
                id = i;
            }
        }
        return id;
    }
    if (policy >> 3 == 0)//FIFO
    {
        int i, id, pol=assoc;
        for (i=0; i<assoc; i++)
        {
            if (cache[i][index].policyControl < pol)
            {
                pol = cache[i][index].policyControl;
                id = i;
            }
        }
        return id;
    }
    if (policy >> 4 == 0)//LIFO
        return assoc-1;
    return 0;
}

/** -------- HACHE --------
 * Funções para se usar na Hache
 */

struct Hache{
    struct Hache *chain;
    int tag;
    char valid;
};

int hash(long long int tag, int nsets)
{
    tag += 1;
    tag *= tag;
    tag /= nsets;
    tag %= nsets;
    return tag;
}

int rehash(int id, int tag, int nsets)
{
    int inc = 1 + (tag % (nsets - 1));
    inc = (id + inc) % nsets;
    return inc;
}

void initializeHache(struct Hache *hache, int nsets)
{
    int i;
    for (i=0; i<nsets; i++)
    {
        hache[i].chain = NULL;
        hache[i].tag = 0;
        hache[i].valid = '\0';
    }
}

void deleteChain(struct Hache *hache)
{
    if (hache->chain != NULL)
        deleteChain(hache->chain);
    free(hache);
}

void deleteHache(struct Hache *hache, int nsets)
{
    int i;
    for (i=0; i<nsets; i++)
        if (hache[i].chain != NULL)
            deleteChain(hache[i].chain);
    free(hache);
}

void addChaining(struct Hache *hache, int id, int tag)
{
    if (hache[id].valid == 0)
    {
        hache[id].tag = tag;
        hache[id].valid |= 0b00000001;
    }
    else
    {
        struct Hache *chain_aux = hache[id].chain;
        while (chain_aux != NULL)
            chain_aux = chain_aux->chain;
        chain_aux = malloc(sizeof(struct Hache));
        chain_aux->chain = NULL;
        chain_aux->tag = tag;
        chain_aux->valid = 1;
    }
}

int help(char *input){
    if(strcmp(input, "help\n") == 0){
        //printf("I need somebody\n\n");
        printf(" ** Simulador de Cache - Kevin e Frederico **\n");
        printf("O simulador de cache deve ser inicializado da seguinte forma:\n");
        printf("\t<politica_de_substituicao> <nsets_L1>:<bsize_L1>:<assoc_L1> arquivo_de_entrada\n\n");
        printf("Para saber mais sobre um determinado parâmetro digite \"help\" seguido por:\n");
        printf("* <politica_de_substituicao> ou <politica>\n* <nsets_L1> ou <nsets>\n* <bsize_L1> ou <bsize>\n* <assoc_L1> ou <assoc>\n* <arquivo_de_entrada> ou <arquivo>\n  Ex: \"help nsets_L1\"\n");
        printf("Mais informações sobre o funcionamento do simulador consulte o relatório que pode ser encontrado em:\n>> http://bit.ly/2Xayout\n\n");
        return 0;
    }
    else if(strcmp(input, "help nsets_L1\n") == 0 || strcmp(input, "help nsets\n") == 0){
        printf("Quantidade de conjuntos para armazenar na cache.\nDefault: 256\n\n");
        return 0;
    }
    else if(strcmp(input, "help bsize_L1\n") == 0 || strcmp(input, "help bsize\n") == 0){
        printf("Tamanho do bloco em bytes de cada endereço da cache.\nDefault: 4\n\n");
        return 0;
    }
    else if(strcmp(input, "help assoc_L1\n") == 0 || strcmp(input, "help assoc\n") == 0){
        printf("Nivel de associatividade dos conjuntos.\nDefault: 1\n\n");
        return 0;
    }
    else if((strcmp(input, "help arquivo_de_entrada\n") == 0) || (strcmp(input, "help arquivo\n") == 0)){
        printf("Nome do arquivo de entrada que armazena todos os enderecos divididos em 4 bytes para a simulacao. Este nome nao pode ser vazio.\n\n");
        return 0;
    }
    else if((strcmp(input, "help politica_de_substituicao\n") == 0) || (strstr(input, "help politica_de_substituicao ") == input) || (strstr(input, "help politica ") == input) || ((strcmp(input, "help politica\n") == 0))){
        if(strstr(input, "LRU"))
            printf("Least Recently Used: substitui o elemento do conjunto que foi chamado a mais tempo\n\n");
        else if(strstr(input, "LFU"))
            printf("Least Frequently Used: substitui o elemento do conjunto que foi chamado menos vezes\n\n");
        else if(strstr(input, "FIFO"))
            printf("First In, First Out: substitui o elemento do conjunto que foi inserido a mais tempo\n\n");
        else if(strstr(input, "LIFO"))
            printf("Last In, First Out: substitui o elemento do conjunto que foi inserido a menos tempo\n\n");
        else if(strstr(input, "RR") || strstr(input, "Random Replacement"))
            printf("Random Replacement: substitui o elemento do conjunto escolhido aleatoriamente\n\n");
        else{
            printf("Configurar a politica de substituição em caches associativas.\nOs macros suportados para configurar esta opção são: 'LRU', 'LFU', 'FIFO' e 'LIFO'.\n");
            printf("O macro padrão, quando nenhum especificado for usado, será 'Random Replacement' ou 'RR'.\n");
            printf("Se a associatividade for igual a 1 ou a palavra inserida não pertencer a estes macros, qualquer parâmetro inserido sera ignorado e o programa usara a opção padrão.\n\n");
            printf("Para saber mais sobre uma politica de substituição digite \"help politica\" ou \"help politica_de_substituicao\" seguido por uma da opções.\n\n");
        }
 
        return 0;
    }
 
    return 1;
}