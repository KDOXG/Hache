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
/*
int hash(long long int tag, int nsets)
{
    tag++;
    long long int hashing = tag * tag;
    hashing = hashing / nsets;
    int modu = hashing % nsets;
    return modu;
}
*/
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
        hache[id].valid = 1;
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