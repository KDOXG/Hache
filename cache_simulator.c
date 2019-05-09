#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ef.c"

struct Miss{
    int compulsory;
    int capacity;
    int conflict;
};

void main()
{
    int hits = 0;
    struct Miss misses;
    misses.compulsory = 0;
    misses.capacity = 0;
    misses.conflict = 0;
    float miss_rate;
    int nsets = 256, bsize = 4, assoc = 1, cache_size;
    char *input_file;
    FILE *input;

    
    /**--------------------- Inicialização
     * O programa deverá receber como entrada uma string no seguinte formato (legenda no .pdf do trabalho):
    /// cache_simulator <nsets_L1>:<bsize_L1>:<assoc_L1> arquivo_de_entrada
     * A saída será os valores passados pela string já formatados para suas respectivas variáveis.
    /*/
    {
        char *input_init, *nsets_aux, *bsize_aux, *assoc_aux;
        input_init = malloc(80*sizeof(char));
        fgets(input_init, 80, stdin);
        getchar();

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
                free(input_init);
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


    cache_size = nsets * bsize * assoc;
    void **Cache = malloc(assoc);
    {
        char **cache_aux = (char**)Cache;
        int i;
        for (i=0; i<assoc; i++)
            cache_aux[i] = malloc(cache_size);
    }


    /**--------------------- Execução
     * O programa exibirá um relatório sobre o acesso à memória.
     */

    printf("
    ########## SIMULADOR DE CPU CACHE ##########\n
    Desenvolvido por: Kevin S. Pereira (KDOXG)\n
    Trabalho de Arquitetura e Organização de Computadores 2\n
    2019\n
    ");

}