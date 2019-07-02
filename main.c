#include "lib_cache.c"

int main(int argc, char *argv[])
{
     if (argc != 4)
    {
        printf("Erro: parâmetros passados de forma incorreta! Retornando...\n\n");
        return -1;
    }

    //Variaveis globais
    int hits = 0, accesses = 0;
    struct Miss misses;
    misses.compulsory = 0;
    misses.capacity = 0;
    misses.conflict = 0;
    float miss_rate;
    struct Hache *hache;
    int nsets = 256, bsize = 4, limit = 16;
    char collision = '\0', *input_init;
    FILE *input;

    /**--------------------- Inicialização
     * O programa deverá receber como entrada uma string no seguinte formato:
     *
     * <tratamento_de_colisoes> <nsets_L1>:<bsize_L1>:<limit_L1> arquivo_de_entrada
     *
     * Legenda:
     * <nsets_L1>: quantidade de conjuntos para armazenar na hache. Este número será dobrado toda vez que ou a hache lotar ou uma das listas encadeadas do Endereçamento Fechado atingir o limite. Valor padrão: 256
     * <bsize_L1>: tamanho do bloco em bytes de cada endereço da hache. Valor padrão: 4
     * <limit_L1>: Define a quantidade limite para uma lista encadeada no Endereçamento Fechado. Valor padrão: 16
     * Caso os parâmetros inseridos não sejam números ou estejam vazios, o programa usará os valores padrões definidos anteriormente.
     *
     * <tratamento_de_colisoes>: escolher o algoritmo para tratar colisões de dados.
     * Os macros suportados para configurar esta opção são:
     * "chaining" - Endereçamento Fechado: insere o elemento no próximo ponteiro vazio disponível na posição retornada pela função hash
     * "overflow" - Endereçamento Aberto Linear: insere o elemento no próximo espaço vazio a seguir da posição retornada pela função hash
     * "re-hash" - Endereçamento Aberto Duplo: insere o elemento no próximo espaço vazio que a função hash encontrar pela posição incrementada com o valor retornado pela função hash auxiliar
     * Se a palavra inserida não pertencer a estes macros, qualquer parâmetro inserido será ignorado e o programa usará a opção "chaining". Este nome não pode ser vazio.
     *
     * arquivo_de_entrada: nome do arquivo de entrada que armazena todos os endereços divididos em 4 bytes para a simulação. Este nome não pode ser vazio.
     *
     * A saída será os valores passados pela string formatados para suas respectivas variáveis.
    /*/

    /** Função:
     * Detectar se a string passada para input_init poderá ser formatada corretamente.
     * Se for possível, extrair da string os números necessários para configurar a hache
     * e o nome do arquivo para simulação.
     */
    {
        input_init = malloc(80*sizeof(char));

        short int i;
        for (i=1; i<argc-1; i++)
        {
            strcat(input_init,argv[i]);
            strcat(input_init, " ");
        }
        strcat(input_init,argv[i]);
        input_init[strlen(input_init)] = '\0';

        short int count_1=0, count_2=0, Error=0;
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
            printf("Erro: parâmetros passados de forma incorreta! Retornando...\n\n");
            free(input_init);
            return -1;
        }
        else
            printf("\n");

        char *collision_aux, *nsets_aux, *bsize_aux, *limit_aux, *input_file;
        short int j=0, k=0;
        collision_aux = input_init;
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
                            limit_aux = input_init+j+1;
                        else
                            limit_aux = NULL;
                    }
                }
                input_init[j] = '\0';
                input_file = (input_init+j+1);
                for (k=0; input_init[k] != '\n'; k++);
                input_init[k] = '\0';
                break;
            }
        }

        if (strcmp(collision_aux,"chaining") == 0)
            collision |= 0;
        if (strcmp(collision_aux,"overflow") == 0)
            collision |= 1;
        if (strcmp(collision_aux,"re-hash") == 0)
            collision |= 2;

        int parametro_1=atoi(nsets_aux), parametro_2=atoi(bsize_aux), parametro_3=atoi(limit_aux);
        if (parametro_1 != 0)
            nsets = parametro_1;
        if (parametro_2 != 0)
            bsize = parametro_2;
        if (parametro_3 != 0)
            limit = parametro_3;

        input = fopen(input_file,"rb");

        free(input_init);
        if (input == NULL)
        {
            printf("Erro: não foi possível ler o arquivo! Retornando...\n\n");
            return -1;
        }
    }

    hache = malloc(nsets*sizeof(struct Hache));
    initializeHache(hache,nsets);

    /**--------------------- Execução
     * O programa deverá receber uma série de valores de entrada, valores estes que serão os endereços procurados
     * posteriormente na matriz criada pelo simulador na inicialização. Cada endereço será um número de 32 bits.
     * A saída será a quantidade de hits e misses ocorridos.
     */

    /** Função:
     * Executará a simulação de buscas de endereços em uma hache de nível 1.
     */
    {
        int tag, index,
        b_offset = ceil(log2(bsize)),
        file_count=0, hache_count=0, count,
        i, id, id_chain;
        Address address;
        struct Hache *hache_aux, *chain_loop;

        while(fgetc(input) != EOF)
        {
            fseek(input,-1,SEEK_CUR);
            fgets((char*)&address.a,5,input);
            file_count += 4;
            fseek(input,file_count,SEEK_SET);
            tag = address.a >> b_offset;
            index = hash(tag,nsets);

            if ((hache[index].valid << 7) >> 7 == 0)//bit de validade
            {
                hache[index].valid |= 0b00000001;
                hache[index].tag = tag;
                hache_count++;
                misses.compulsory++;
                continue;
            }

            if (hache[index].tag == tag)
            {
                hits++;
                continue;
            }

            if (collision >> 0 == 0)//chaining
            {
                count = 0;
                chain_loop = hache[index].chain;
                while(chain_loop != NULL)
                {
                    if (chain_loop->tag == tag)
                    {
                        hits++;
                        break;
                    }
                    else
                    {
                        chain_loop = chain_loop->chain;
                        count++;
                    }
                }

                if (chain_loop == NULL)
                {
                    if (count != limit)
                    {
                        chain_loop = &hache[index];
                        while(chain_loop->chain != NULL)
                            chain_loop = chain_loop->chain;
                        chain_loop->chain = malloc(sizeof(struct Hache));
                        chain_loop->chain->chain = NULL;
                        chain_loop->chain->tag = tag;
                        chain_loop->chain->valid |= 0b00000001;
                        hache_count++;
                        misses.conflict++;
                        continue;
                    }
                    else
                    {
                        nsets *= 2;
                        hache_aux = malloc(nsets*sizeof(struct Hache));
                        initializeHache(hache_aux,nsets);
                        for (i=0; i<(nsets/2); i++)
                        {
                            id = hash(hache[i].tag,nsets);
                            addChaining(hache_aux,id,hache[i].tag);
                            chain_loop = hache[i].chain;
                            while(chain_loop != NULL)
                            {
                                id_chain = hash(chain_loop->tag,nsets);
                                addChaining(hache_aux,id_chain,chain_loop->tag);
                                chain_loop = chain_loop->chain;
                            }
                        }
                        deleteHache(hache,nsets/2);
                        hache = hache_aux;

                        id = hash(tag,nsets);
                        addChaining(hache,id,tag);
                        misses.capacity++;
                        hache_count++;
                    }
                }
                continue;
            }

            if (collision >> 1 == 0)//overflow
            {
                for (id=index; id < nsets && (hache[id].valid << 7) >> 7 == 1; id++)
                {
                    if (hache[id].tag == tag)
                    {
                        hits++;
                        break;
                    }
                }
                if (id < nsets && hache[id].tag != tag)
                {
                    hache[id].tag = tag;
                    hache[id].valid |= 0b00000001;
                    misses.conflict++;
                    hache_count++;
                    continue;
                }
                if (id == nsets)
                {
                    nsets *= 2;
                    hache_aux = malloc(nsets*sizeof(struct Hache));
                    initializeHache(hache_aux,nsets);
                    for (i=0; i<(nsets/2); i++)
                    {
                        if ((hache[i].valid << 7) >> 7 == 1)
                            id = hash(hache[i].tag,nsets);
                        else
                            continue;
                        if ((hache_aux[id].valid << 7) >> 7 != 1)
                        {
                            hache[id].tag = hache[i].tag;
                            hache[id].valid |= 0b00000001;
                            continue;
                        }
                        while ((hache_aux[id].valid << 7) >> 7 == 1)
                            id++;
                        hache_aux[id].tag = hache[i].tag;
                        hache_aux[id].valid |= 0b00000001;
                    }
                    free(hache);
                    hache = hache_aux;

                    id = hash(tag,nsets);
                    while ((hache[id].valid << 7) >> 7 == 1)
                        id++;
                    hache[id].tag = tag;
                    hache[id].valid |= 0b00000001;
                    misses.capacity++;
                    hache_count++;
                }
                continue;
            }

            if (collision >> 2 == 0)//re-hash
            {
                id = index;
                count = 0;
                while (hache[id].tag != tag && (hache[id].valid << 7) >> 7 == 1 && count < nsets)
                {
                    id = rehash(id,tag,nsets);
                    count++;
                }
                if (hache[id].tag == tag)
                {
                    hits++;
                    continue;
                }
                if (count < nsets)
                {
                    hache[id].tag = tag;
                    hache[id].valid |= 0b00000001;
                    misses.conflict++;
                    hache_count++;
                    continue;
                }
                else
                {
                    nsets *= 2;
                    hache_aux = malloc(nsets*sizeof(struct Hache));
                    initializeHache(hache_aux,nsets);
                    for (i=0; i<(nsets/2); i++)
                    {
                        if ((hache[i].valid << 7) >> 7 == 1)
                            id = hash(hache[i].tag,nsets);
                        else
                            continue;
                        if ((hache_aux[id].valid << 7) >> 7 != 1)
                        {
                            hache[id].tag = hache[i].tag;
                            hache[id].valid |= 0b00000001;
                            continue;
                        }
                        while ((hache_aux[id].valid << 7) >> 7 == 1)
                            id = rehash(id,hache[i].tag,nsets);
                        hache_aux[id].tag = hache[i].tag;
                        hache_aux[id].valid |= 0b00000001;
                    }
                    free(hache);
                    hache = hache_aux;

                    id = hash(tag,nsets);
                    while ((hache[id].valid << 7) >> 7 == 1)
                        id = rehash(id,tag,nsets);
                    hache[id].tag = tag;
                    hache[id].valid |= 0b00000001;
                    misses.capacity++;
                    hache_count++;
                }
            }
        }
        accesses = misses.capacity + misses.compulsory + misses.conflict + hits;
        miss_rate = 100 * (accesses-hits) / accesses;
        deleteHache(hache,nsets);
        fclose(input);
    }

    /**--------------------- Finalização
     * O programa exibirá um relatório sobre o acesso à memória.
     */

    printf("########## SIMULADOR DE HACHE ##########\n");
    printf("Desenvolvido por: Kevin Pereira (KDOXG)\n");
    printf("Projeto final para Algoritmos e Estruturas de Dados 2\n\n");
           

    printf("Objetivo da Hache:\n");

    printf("Uma hache é uma tabela hash que contabiliza seus acessos de maneira similar a um simulador de memória cache (\"hash\" + \"cache\" = \"hache\")\n");
    printf("A hache contabiliza quantos acertos uma tabela hash construída com os parâmetros inseridos pelo usuario pode executar, e quantos erros ela pode cometer.\n");
    printf("Os erros são classificados em:\n");
    printf("Conflito: quando um endereço é calculado para um novo dado mas tal endereço já está sendo ocupado por outro dado.\n");
    printf("Um novo endereço é recalculado de acordo com o tratamento de colisões escolhido.\n");
    printf("Capacidade: quando não há mais espaço disponível para novos dados.\n");
    printf("Uma nova hache é alocada com o dobro do tamanho para suprir a quantidade.\n");
    printf("Tais dados obtidos podem ser usados para fins estatísticos e análise de desempenho para uma tabela hash equivalente.\n\n");
    printf("Tamanho da hache: %d elementos\n", nsets);
    printf("Tratamento de colisões: ");
    if (collision >> 0 == 0)
        printf("Endereçamento Fechado (Chaining) com limite %d\n", limit);
    else if (collision >> 1 == 0)
        printf("Endereçamento Aberto Linear (Overflow)\n");
    else if (collision >> 1 == 0)
        printf("Endereçamento Aberto Duplo (Re-hash)\n");

    printf("Função hash(Tag_Endereço,Tamanho): (Tag_Endereço + 1)² / Tamanho mod Tamanho\n");
    printf("Função rehash(Tag_Endereço,Tamanho): (hash(Tag_Endereço,Tamanho) + (1 + Tag_Endereço mod (Tamanho - 1))) mod Tamanho\n");
    printf("Quantidade de acessos: %d\n", accesses);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses.capacity + misses.compulsory + misses.conflict);
    printf("Misses compulsórios: %d\n", misses.compulsory);
    printf("Misses de conflito: %d\n", misses.conflict);
    printf("Misses de capacidade: %d\n", misses.capacity);
    printf("Taxa de miss: %.2f%%", miss_rate);
    printf("\nPrograma encerrado corretamente! Retornando...\n\n");

    return 0;
}
