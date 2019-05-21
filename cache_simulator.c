#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ef.c"

struct Miss{
    int compulsory;
    int capacity;
    int conflict;
};

struct Cache{
    int tag;
    int policyControl;
    char bit_valid;
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
        cache[i][index].policyControl = assoc-1;
        int j;
        for (j=0; j<assoc; j++)
        {
            if (cache[j][index].policyControl != 0 && j != i)
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
        int i, id, pol=0;
        for (i=0; i<assoc; i++)
        {
            if (cache[i][index].policyControl > pol)
                pol = cache[i][index].policyControl;
        }
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

void main()
{
    //Variaveis globais
    int hits = 0, accesses = 0;
    struct Miss misses;
    misses.compulsory = 0;
    misses.capacity = 0;
    misses.conflict = 0;
    float miss_rate;
    struct Cache **cache;
    int nsets = 256, bsize = 4, assoc = 1;
    char policy = '\0', *input_init;
    FILE *input;

    /**--------------------- Inicialização
     * O programa deverá receber como entrada uma string no seguinte formato:
     *
     * <politica_de_substituicao> <nsets_L1>:<bsize_L1>:<assoc_L1> arquivo_de_entrada
     *
     * Exemplos:
     * cache_simulator 1024:4:1 test.bin
     * LRU 512:4:2 test.bin
     * FIFO 256:4:4 test.bin
     * default 1:4:1024 test.bin
     *
     * Legenda:
     * <nsets_L1>: quantidade de conjuntos para armazenar na cache. Valor padrão: 256
     * <bsize_L1>: tamanho do bloco de cada endereço da cache. Valor padrão: 4
     * <assoc_L1>: nível de associatividade. Valor padrão: 1
     * Caso os parâmetros inseridos não sejam números ou estejam vazios, o programa usará os valores padrões definidos anteriormente.
     * (Para melhor aproveitamento do espaço da cache, sugerimos sempre usar números potências de 2.)
     *
     * <politica_de_substituicao>: configurar a política de substituição em caches associativas.
     * Os macros suportados para configurar esta opção são:
     * "LRU" - Least Recently Used: substitui o elemento do conjunto que foi chamado a mais tempo
     * "LFU" - Least Frequently Used: substitui o elemento do conjunto que foi chamado menos vezes
     * "FIFO" - First In, First Out: substitui o elemento do conjunto que foi inserido a mais tempo
     * "LIFO" - Last In, First Out: substitui o elemento do conjunto que foi inserido a menos tempo
     * "default" - usará a política de substituição padrão do programa, Random Replacement: substitui o elemento do conjunto escolhido aleatoriamente
     * Se a associatividade for igual a 1 ou a palavra inserida não pertencer a estes macros, qualquer parâmetro inserido será ignorado e o programa usará a opção "default".
     *
     * arquivo_de_entrada: nome do arquivo de entrada que armazena todos os endereços divididos em 4 bytes para a simulação.
     *
     * A saída será os valores passados pela string formatados para suas respectivas variáveis.
    /*/

    input_init = malloc(80*sizeof(char));
    fgets(input_init, 80, stdin);

    /** Função:
     * Detectar se a string passada para input_init poderá ser formatada corretamente.
     */
    {
        short int i, count_1=0, count_2=0, Error=0;
        for (i=0; input_init[i] != '\0'; i++)
        {
            if (input_init[i] == ' ' && i == 0)
                Error = 1;
            if (input_init[i] == ' ')
                count_1++;
            if (count_1 == 1 && input_init[i] == ':')
                count_2++;
            if (count_1 == 2 && (input_init[i+1] == '\0' || input_init[i+1] == '\n'))
                Error = 1;
            if (count_1 == 2 && input_init[i+1] != '\0' && input_init[i+1] != '\n' && input_init[i+1] != ' ')
                break;
        }
        if (Error != 0 || count_1 != 2 || count_2 != 2)
        {
            printf("Erro: parâmetros passados de forma incorreta! Desligando...");
            free(input_init);
            return;
        }
        else
            printf("\n\n");
    }

    /** Função:
     * Extrair da string os números necessários para configurar a cache
     * e o nome do arquivo para simulação.
     */
    {
        char *policy_aux, *nsets_aux, *bsize_aux, *assoc_aux, *input_file;
        short int i, j=0, k=0;
        policy_aux = input_init;
        for (i=0; i<strlen(input_init); i++)
        {
            if (input_init[i] == ' ' && j == 0)
            {
                input_init[i] = '\0';
                if (input_init[i+1] != ':')
                    nsets_aux = input_init+i+1;
                else
                    nsets_aux = NULL;

                for (j=i+1; input_init[j] != ' '; j++)
                {
                    if (input_init[j] == ':' && k == 0)
                    {
                        input_init[j] = '\0';
                        if (input_init[j+1] != ':')
                            bsize_aux = input_init+j+1;
                        else
                            bsize_aux = NULL;
                        k=1;
                    }
                    if (input_init[j] == ':' && k == 1)
                    {
                        input_init[j] = '\0';
                        if (input_init[j+1] != ' ')
                            assoc_aux = input_init+j+1;
                        else
                            assoc_aux = NULL;
                    }
                }
                input_init[j] = '\0';
                input_file = (input_init+j+1);
                for (k=0; input_init[k] != '\n'; k++);
                input_init[k] = '\0';
                break;
            }
        }

        if (strcmp(policy_aux,"LRU") == 0)
            policy |= 1;
        if (strcmp(policy_aux,"LFU") == 0)
            policy |= 2;
        if (strcmp(policy_aux,"FIFO") == 0)
            policy |= 4;
        if (strcmp(policy_aux,"LIFO") == 0)
            policy |= 8;

        if (strcmp(nsets_aux,"eu") == 0 && strcmp(bsize_aux,"quero") == 0 && strcmp(assoc_aux,"hash") == 0)
            hache(); //Surpresinha~

        int parametro_1=atoi(nsets_aux), parametro_2=atoi(bsize_aux), parametro_3=atoi(assoc_aux);
        if (parametro_1 != NULL)
            nsets = parametro_1;
        if (parametro_2 != NULL)
            bsize = parametro_2;
        if (parametro_3 != NULL)
            assoc = parametro_3;
        if (assoc == 1)
            policy = '\0';

        input = fopen(input_file,"rb");
    }
    free(input_init);
    if (input == NULL)
    {
        printf("Erro: não foi possível ler o arquivo! Desligando...");
        return;
    }

    /** Função:
     * Inicializar a matriz recém alocada que simulará a memória cache.
     */
    cache = malloc(assoc*sizeof(struct Cache*));
    {
        int i,j;
        for (i=0; i<assoc; i++)
        {
            cache[i] = malloc(nsets*sizeof(struct Cache));
            for (j=0; j<nsets; j++)
            {
                cache[i][j].tag = 0;
                cache[i][j].policyControl = 0;
                cache[i][j].bit_valid = 0;
            }
        }
    }

    /**--------------------- Execução
     * O programa deverá receber uma série de valores de entrada, valores estes que serão os endereços procurados
     * posteriormente na matriz criada pelo simulador na inicialização. Cada endereço será um número de 32 bits.
     * A saída será a quantidade de hits e misses ocorridos.
     */

    /** Função:
     * Executará a simulação de buscas de endereços em uma cache de nível 1.
     */
    {
        int tag, index,
        b_offset = ceil(log2(bsize)), b_index = ceil(log2(nsets)),
        i, file_count=0, cache_count=0;
        Address address;

        while(fgetc(input) != EOF)
        {
            fseek(input,-1,SEEK_CUR);
            fgets((char*)&address.a,5,input);
            file_count += 4;
            fseek(input,file_count,SEEK_SET);
            tag = address.a >> (b_offset + b_index);
            index = (address.a >> (b_offset)) & (int)(pow(2,b_index)-1);
            for (i=0; i<assoc; i++)
            {
                if (cache[i][index].bit_valid == 0)
                {
                    cache[i][index].bit_valid = 1;
                    cache[i][index].tag = tag;
                    cache_count++;
                    misses.compulsory++;
                    setReplacement(cache,policy,i,assoc,index,0b00000010);
                    break;
                }
                if (cache[i][index].tag == tag)
                {
                    hits++;
                    setReplacement(cache,policy,i,assoc,index,0b00000000);
                    break;
                }
            }

            if (i == assoc)
            {

                i = getReplacement(cache,policy,assoc,index);
                cache[i][index].tag = tag;
                setReplacement(cache,policy,i,assoc,index,0b00000001);
                if (cache_count == (nsets*assoc))
                    misses.capacity++;
                else
                    misses.conflict++;
            }
        }
        accesses = misses.capacity + misses.compulsory + misses.conflict + hits;
        miss_rate = 100 * (accesses-hits) / accesses;

        for (i=0; i<assoc; i++)
            free(cache[i]);
        free(cache);
        fclose(input);
    }

    /**--------------------- Finalização
     * O programa exibirá um relatório sobre o acesso à memória.
     */

    printf("########## SIMULADOR DE CPU CACHE ##########\nDesenvolvido por: Kevin S. Pereira (KDOXG)\nTrabalho de Arquitetura e Organização de Computadores 2\nUniversidade Federal de Pelotas, 2019\n\n");

    printf("Tamanho da memória cache: %d bytes\n", nsets * bsize * assoc);
    printf("Mapeamento: ");
    if (assoc == 1)
        printf("Direto\n");
    else
    {
        if (nsets != 1)
            printf("Conjunto Associativo de %d vias\n", assoc);
        else
            printf("Totalmente Associativo\n");
        printf("Política de substituição: ");
        while(1)
        {///Acho que esta foi a maior POG da minha vida o_O'
            if (policy >> 0 == 0)
            {
                printf("Random Replacement (RR)\n");
                break;
            }
            if (policy >> 1 == 0)
            {
                printf("Least Recently Used (LRU)\n");
                break;
            }
            if (policy >> 2 == 0)
            {
                printf("Least Frequently Used (LFU)\n");
                break;
            }
            if (policy >> 3 == 0)
            {
                printf("First In, First Out (FIFO)\n");
                break;
            }
            if (policy >> 4 == 0)
            {
                printf("Last In, First Out (LIFO)\n");
                break;
            }
            break;
        }
    }
    printf("Quantidade de acessos: %d\n", accesses);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses.capacity + misses.compulsory + misses.conflict);
    printf("Misses compulsórios: %d\n", misses.compulsory);
    printf("Misses de conflito: %d\n", misses.conflict);
    printf("Misses de capacidade: %d\n", misses.capacity);
    printf("Taxa de miss: %.2f%%", miss_rate);
    printf("\nPrograma encerrado corretamente! Reiniciando...\n\n");
    main();
}
