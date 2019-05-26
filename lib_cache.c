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