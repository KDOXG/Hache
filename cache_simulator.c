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
    char bit_valid;
};

typedef struct Endereco{
    int a;
    char nuu;
}Address;

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
    char *input_init, *input_file;
    FILE *input;

    /**--------------------- Inicialização
     * O programa deverá receber como entrada uma string no seguinte formato (legenda no .pdf do trabalho):
     *
     * cache_simulator <nsets_L1>:<bsize_L1>:<assoc_L1> arquivo_de_entrada
     *
     * Exemplos:
     * cache_simulator 1024:4:1 test.bin
     * cache_simulator 512:4:2 test.bin
     * cache_simulator 256:4:4 test.bin
     * cache_simulator 1:4:1024 test.bin
     *
     * A saída será os valores passados pela string formatados para suas respectivas variáveis.
    /*/

    getchar();
    input_init = malloc(80*sizeof(char));
    fgets(input_init, 80, stdin);

    /** Função:
     * Detectar se a string passada para input_init poderá ser formatada corretamente.
     */
    {
        short int i, count_1=0, count_2=0, isFilenameEmpty=0;
        for (i=0; input_init[i] != '\0'; i++)
        {
            if (input_init[i] == ' ')
                count_1++;
            if (count_1 == 1 && input_init[i] == ':')
                count_2++;
            if (count_1 == 2 && input_init[i+1] == '\0' && input_init[i+1] == '\n' && input_init[i+1] == ' ')
                isFilenameEmpty = 1;
            if (count_1 == 2 && input_init[i+1] != '\0' && input_init[i+1] != '\n' && input_init[i+1] != ' ')
                break;
        }
        if (isFilenameEmpty == 1 || count_1 != 2 || count_2 != 2)
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
        char *nsets_aux, *bsize_aux, *assoc_aux;
        short int i, j=0, k=0;
        for (i=0; i<strlen(input_init); i++)
        {
            if (input_init[i] == ' ' && j == 0)
            {
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
                input = fopen(input_file,"rb");
                break;
            }
        }

        if (strcmp(nsets_aux,"owen") == 0 && strcmp(bsize_aux,"was") == 0 && strcmp(assoc_aux,"her?") == 0)
            EF(); //heh

        int parametro_1=atoi(nsets_aux), parametro_2=atoi(bsize_aux), parametro_3=atoi(assoc_aux);
        if (parametro_1 != NULL)
            nsets = parametro_1;
        if (parametro_2 != NULL)
            bsize = parametro_2;
        if (parametro_3 != NULL)
            assoc = parametro_3;
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
        int tag, index,/* offset,*/
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
            //offset = address.a & (pow(2,b_offset)-1);
            for (i=0; i<assoc; i++)
            {
                if (cache[i][index].bit_valid == 0)
                {
                    cache[i][index].bit_valid = 1;
                    cache[i][index].tag = tag;
                    cache_count++;
                    misses.compulsory++;
                    break;
                }
                if (cache[i][index].tag == tag)
                {
                    hits++;
                    break;
                }
            }

            if (i == assoc)
            {
                i = rand() % assoc;
                cache[i][index].tag = tag;
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
