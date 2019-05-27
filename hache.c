#include "lib_cache.c"
/*
void setHandling(struct Hache *hache, char collision, int nsets, int limit, char flag)
{
    return hash();
}

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

int getHandling(struct Hache *hache, char collision, int nsets, char flag)
{
    if (collision >> 0 == 0)//chaining
    {

    }
    if (collision >> 1 == 0)//overflow
    {

    }
    if (collision >> 2 == 0)//re-hash
    {

    }
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
*/
void hache()
{
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
     * Exemplos:
     * hache_simulator 1024:4:4 test.bin
     * hache 4:1:4 byte.bin
     * chaining 512:8:8 test.bin
     * overflow 256:4:16 test.bin
     * re-hash 1:2:256 test.bin
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
            printf("Erro: parâmetros passados de forma incorreta! Retornando...");
            free(input_init);
            return;
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
            printf("Erro: não foi possível ler o arquivo! Retornando...");
            return;
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
        b_offset = ceil(log2(bsize)),/* b_index = ceil(log2(nsets)),*/
        file_count=0, hache_count=0, chain_count, i, id, id_chain;
        char flag = '\0'; //Preciso lembrar para o quê eu queria esta variável .-. Acho que era a função para diferenciar qual o tipo de colisão o usuário escolheu, mas já adaptei pro chaining, então não acho que eu vou querer pros outros...
        if (collision >> 0 == 0)//chaining
            flag |= 1;
        if (flag == '\0' && collision >> 1 == 0)//overflow
            flag |= 2;
        if (flag == '\0' && collision >> 2 == 0)//re-hash
            flag |= 4;
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
                hache[index].valid |= 1;
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
                chain_count = 0;
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
                        chain_count++;
                    }   
                }

                if (chain_loop == NULL)
                {
                    if (chain_count != limit)
                    {
                        chain_loop = &hache[index];
                        while(chain_loop->chain != NULL)
                            chain_loop = chain_loop->chain;
                        chain_loop->chain = malloc(sizeof(struct Hache));
                        chain_loop->chain->chain = NULL;
                        chain_loop->chain->tag = tag;
                        chain_loop->chain->valid = 1;
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
                        if (hache_count == nsets*limit) 
                            misses.capacity++;
                        else
                            misses.conflict++;
                        hache_count++;
                    }
                }
                continue;
            }
            
            if (collision >> 1 == 0)//overflow
            {

            }
            if (collision >> 2 == 0)//re-hash
            {

            }
/*
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
*/
        }
        accesses = misses.capacity + misses.compulsory + misses.conflict + hits;
        miss_rate = 100 * (accesses-hits) / accesses;
        /*
        for (i=0; i<assoc; i++)
            free(cache[i]);
        free(cache);
        fclose(input);
        */
    }

    /**--------------------- Finalização
     * O programa exibirá um relatório sobre o acesso à memória.
     */

    printf("########## SIMULADOR DE CPU HACHE ##########\nDesenvolvido por: Senhor K (KDOXG)\nProjeto paralelo e pessoal\n(e também bônus secreto do meu 2º trabalho de AOC2)\n\n");

    printf("Tamanho da memória cache: %d bytes\n", nsets * bsize * limit);
    printf("Mapeamento ");
    if (limit == 1)
        printf("Direto\n");
    else
    {
        if (nsets != 1)
            printf("Conjunto-Associativo de %d vias\n", limit);
        else
            printf("Totalmente Associativo\n");
        printf("Política de substituição: ");
        while(1)
        {///Acho que esta foi a maior POG da minha vida o_O'
            if (collision >> 0 == 0)
            {
                printf("Random Replacement (RR)\n");
                break;
            }
            if (collision >> 1 == 0)
            {
                printf("Least Recently Used (LRU)\n");
                break;
            }
            if (collision >> 2 == 0)
            {
                printf("Least Frequently Used (LFU)\n");
                break;
            }
            if (collision >> 3 == 0)
            {
                printf("First In, First Out (FIFO)\n");
                break;
            }
            if (collision >> 4 == 0)
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
    printf("\nPrograma encerrado corretamente! Retornando...\n\n");
    
    return;
}
