#include <time.h>
#include "hache.c"

void wait(int seconds)
{
    int aux=0;
    int n = seconds * 1024;
    clock_t start_time = clock();
    while (clock() < start_time + n)
    {
        aux=aux;
    }
}

void main()
{
    //Variaveis globais
    int hits, accesses;
    struct Miss misses;
    float miss_rate;
    struct Cache **cache;
    int nsets = 256, bsize = 4, assoc = 1;
    char policy;
    FILE *input;

    /**--------------------- Inicialização
     * O programa deverá receber como entrada uma string no seguinte formato:
     *
     * <politica_de_substituicao> <nsets_L1>:<bsize_L1>:<assoc_L1> arquivo_de_entrada
     *
     * Legenda:
     * <nsets_L1>: quantidade de conjuntos para armazenar na cache. Valor padrão: 256
     * <bsize_L1>: tamanho do bloco em bytes de cada endereço da cache. Valor padrão: 4
     * <assoc_L1>: nível de associatividade dos conjuntos. Valor padrão: 1
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
     * arquivo_de_entrada: nome do arquivo de entrada que armazena todos os endereços divididos em 4 bytes para a simulação. Este nome não pode ser vazio.
     *
     * A saída será os valores passados pela string formatados para suas respectivas variáveis.
    /*/

    /** Função:
     * Inicializar todas as variáveis globais declaradas no topo.
     * Depois, detectar se a string passada para input_init poderá ser formatada corretamente.
     * Se for possível, extrair da string os números necessários para configurar a cache
     * e o nome do arquivo para simulação.
     */
    Cache:
    hits = 0;
    accesses = 0;
    misses.compulsory = 0;
    misses.capacity = 0;
    misses.conflict = 0;
    nsets = 256;
    bsize = 4;
    assoc = 1;
    policy = '\0';

    {
        char *input_init = malloc(80*sizeof(char));
        fgets(input_init, 80, stdin);

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
            wait(3);
            return;
        }
        else
            printf("\n");

        char *policy_aux, *nsets_aux=NULL, *bsize_aux=NULL, *assoc_aux=NULL, *input_file;
        short int j=0, k=0, code=1;
        policy_aux = input_init;
        for (i=0; i<strlen(input_init); i++)
        {
            if (input_init[i] == ' ' && j == 0)
            {
                input_init[i] = '\0';
                if (input_init[i+1] != ':')
                    nsets_aux = input_init+i+1;
                else
                    code=0;

                for (j=i+1; input_init[j] != ' '; j++)
                {
                    if (input_init[j] == ':' && k == 0)
                    {
                        input_init[j] = '\0';
                        if (input_init[j+1] != ':')
                            bsize_aux = input_init+j+1;
                        else
                            code=0;
                        k=1;
                    }
                    if (input_init[j] == ':' && k == 1)
                    {
                        input_init[j] = '\0';
                        if (input_init[j+1] != ' ')
                            assoc_aux = input_init+j+1;
                        else
                            code=0;
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

        int parametro_1=atoi(nsets_aux), parametro_2=atoi(bsize_aux), parametro_3=atoi(assoc_aux);
        if (parametro_1 != 0)
            nsets = parametro_1;
        if (parametro_2 != 0)
            bsize = parametro_2;
        if (parametro_3 != 0)
            assoc = parametro_3;
        if (assoc == 1)
            policy = '\0';
        if (code)
            if (strcmp(nsets_aux,"eu") == 0 && strcmp(bsize_aux,"quero") == 0 && strcmp(assoc_aux,"hash") == 0)
            {
                free(input_init);
                hache();
                goto Cache;
            }

        input = fopen(input_file,"rb");

    	free(input_init);
        if (input == NULL)
        {
            printf("Erro: não foi possível ler o arquivo! Desligando...");
            wait(3);
            return;
        }
    }

    /** Função:
     * Inicializar a matriz recém alocada que simulará a memória cache.
     */
    {
        int i,j;
        cache = malloc(assoc*sizeof(struct Cache*));
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

    printf("########## SIMULADOR DE CPU CACHE ##########\nTrabalho de Arquitetura e Organização de Computadores 2\nUniversidade Federal de Pelotas, 2019\n\n");

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

    goto Cache;
}
