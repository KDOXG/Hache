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

void main()
{
    //Variaveis globais
    int hits = 0, accesses = 0;
    struct Miss misses;
    misses.compulsory = 0;
    misses.capacity = 0;
    misses.conflict = 0;
    float miss_rate;
    int nsets = 256, bsize = 4, assoc = 1, cache_size;
    char *input_init, *input_file;
    FILE *input;
    
    /**--------------------- Inicialização
     * O programa deverá receber como entrada uma string no seguinte formato (legenda no .pdf do trabalho):
     * 
     * cache_simulator <nsets_L1>:<bsize_L1>:<assoc_L1> arquivo_de_entrada
     * 
     * A saída será os valores passados pela string formatados para suas respectivas variáveis.
    /*/
    
    input_init = malloc(80*sizeof(char));
    getchar();
    fgets(input_init, 80, stdin);
    {
        char *nsets_aux, *bsize_aux, *assoc_aux;

        //Criar funcao que verifique se a string de configuracao esta bem escrita

        int i;
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
                        input[j] = '\0';
                        if (input_init[j+1] != ' ')
                            assoc_aux = input_init+j+1;
                        else
                            assoc_aux = NULL;
                    }
                }
                input_init[j] = '\0';

                if (nsets_aux != NULL)
                {
                    char *nsets_aux_temp = nsets_aux;
                    nsets_aux = malloc(strlen(nsets_aux)+1);
                    strcpy(nsets_aux,nsets_aux_temp);
                }
                if (bsize_aux != NULL)
                {
                    char *bsize_aux_temp = bsize_aux;
                    bsize_aux = malloc(strlen(bsize_aux)+1);
                    strcpy(bsize_aux,bsize_aux_temp);
                }
                if (assoc_aux != NULL)
                {
                    char *assoc_aux_temp = assoc_aux;
                    assoc_aux = malloc(strlen(assoc_aux)+1);
                    strcpy(assoc_aux,assoc_aux_temp);
                }
                i = j+1;
                input_file = malloc(strlen(input_init+i+1)+1);
                strcpy(input_file,input_init+i+1);
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
    cache_size = nsets * bsize;
    struct Cache **cache = malloc(assoc*sizeof(struct Cache));
    {
        int i,j;
        for (i=0; i<assoc; i++)
        {
            cache[i] = malloc(cache_size*sizeof(struct Cache));
            for (j=0; j<cache_size; j++)
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
    {
        int address, tag, index, offset,
        b_offset = log2(bsize), b_index = log2(nsets),
        i, cache_count=0;
        char *endereco;
        ///Implementar leitura do arquivo aqui
        while(/*!EOF*/)
        {
            //endereco = endereco_do_arquivo;
            ///Implementar incremento do ponteiro do arquivo aqui para poder pegar o proximo endereco no proximo loop
            address = atoi(endereco);
            tag = address >> (b_offset + b_index);
            index = address >> (b_offset & (pow(2,b_index)-1));
            hit_flag = 0;
            miss_flag = 0;
            for (i=0; i<assoc; i++)
            {
                if (cache[i][b_index].bit_valid == 0)
                {
                    cache[i][b_index].bit_valid = 1;
                    cache[i][b_index].tag = tag;
                    cache_count++;
                    misses.compulsory++;
                    break;
                }
                if (cache[i][b_index].tag == tag)
                {
                    hits++;
                    break;
                }
            }

            if (i == assoc)
            {
                i = rand() % assoc;
                cache[i][b_index].tag = tag;
                if (cache_count == cache_size)
                    misses.capacity++;
                else
                    misses.conflict++;
            }
        }
        accesses = misses.capacity + misses.compulsory + misses.conflict + hits;
        miss_rate = 100 * (accesses-hits) / accesses;
    }

    /**--------------------- Finalização
     * O programa exibirá um relatório sobre o acesso à memória.
     */

    printf("
    ########## SIMULADOR DE CPU CACHE ##########\n
    Desenvolvido por: Kevin S. Pereira (KDOXG)\n
    Trabalho de Arquitetura e Organização de Computadores 2\n
    2019\n
    \n");

    printf("Tamanho da memória cache: %d bytes\n", cache_size * assoc);
    printf("Quantidade de acessos: %d\n", accesses);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses.capacity + misses.compulsory + misses.conflict);
    printf("Misses compulsórios: %d\n", misses.compulsory);
    printf("Misses de conflito: %d\n", misses.conflict);
    printf("Misses de capacidade: %d\n", misses.capacity);
    printf("Taxa de miss: %f%%", miss_rate);
}
