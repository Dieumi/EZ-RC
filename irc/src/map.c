#include "map.h"

HashMap* HashMap_create(unsigned int slots, double loadFactor, double growFactor)
{
    int i;
    HashMap *map = (HashMap*) malloc(sizeof(HashMap));
    map->loadFactor = loadFactor;
    map->growFactor = growFactor;
    map->slots = slots;
    map->entries = (HashMapEntry**) malloc(slots * sizeof(HashMapEntry*));
    map->size = 0;

    for(i = 0 ; i < slots ; i++)
    {
        map->entries[i] = NULL;
    }

    return map;
}

void HashMap_put(HashMap* map, char* key, void* value, unsigned int elemSize)
{
    if(map == NULL)
        return;
    int hash = hashCode(key) % map->slots;
    HashMapEntry* entry = HashMap_get(map, key);

    if(entry != NULL)
    {
        HashMapEntry_update(entry, value, elemSize);
    }
    else
    {
        if(map->size >= map->slots * map->loadFactor)
        {
            map = HashMap_extend(map);
            hash = hashCode(key) % map->slots;
        }
        map->entries[hash] = CL_append(map->entries[hash], key, value, elemSize);
        map->size++;
    }
}

HashMap* HashMap_extend(HashMap* map)
{
    int i;
    int newHashKey;
    HashMapEntry* entryList = NULL;
    unsigned int newSlotNumber = map->slots * map->growFactor;
    HashMapEntry** newEntries = (HashMapEntry**) malloc(newSlotNumber * sizeof(HashMapEntry*));

    for(i = 0 ; i < newSlotNumber ; i++)
    {
        newEntries[i] = NULL;
    }
    for(i = 0 ; i < map->slots ; i++)
    {
        entryList = map->entries[i];
        while(entryList != NULL)
        {
            newHashKey = hashCode(entryList->key) % newSlotNumber;
            newEntries[newHashKey] = CL_append(newEntries[newHashKey], entryList->key, entryList->value, entryList->elemSize);
            entryList = entryList->next;
        }
        HashMapEntry_freeList(&(map->entries[i]));
    }
    free(map->entries);
    map->slots = newSlotNumber;
    map->entries = newEntries;
    return map;
}

HashMapEntry* HashMap_get(HashMap* map, char* key)
{
    if(map == NULL)
        return NULL;
    int hash = hashCode(key) % map->slots;
    HashMapEntry* entryList = map->entries[hash];

    while(entryList != NULL)
    {
        if(!strcmp(key, entryList->key))
        {
            return entryList;
        }
        entryList = entryList->next;
    }

    return entryList;
}

void HashMap_remove(HashMap* map, char* key)
{
    if(map == NULL)
        return;
    int hash = hashCode(key) % map->slots;
    HashMapEntry** entry = &(map->entries[hash]);

    while(*entry != NULL)
    {
        if(strcmp((*entry)->key, key) == 0) {
            HashMapEntry* del = *entry;
            *entry = (*entry)->next;
            free(del);
            map->size--;
            return;
        }
        *entry = (*entry)->next;
    }
}

void HashMapEntry_update(HashMapEntry* entry, void* value, unsigned int elemSize)
{
    if(entry != NULL)
    {
        free(entry->value);
        entry->value = malloc(elemSize);
        memcpy(entry->value, value, elemSize);
        entry->elemSize = elemSize;
    }
}

void HashMapEntry_freeList(HashMapEntry** entry)
{
    HashMapEntry* next = NULL;
    while(*entry != NULL)
    {
        next = (*entry)->next;
        free((*entry)->value);
        free((*entry)->key);
        free(*entry);
        *entry = next;
    }
}

void HashMap_free(HashMap** map)
{
    if(*map != NULL)
    {
        int i = 0;
        for(i = 0 ; i < (*map)->slots ; i++)
        {
            HashMapEntry_freeList(&((*map)->entries[i]));
        }
        free((*map)->entries);
        free(*map);
    }
}

void HashMapEntry_print(HashMapEntry* entry)
{
    if(entry != NULL)
    {
        char* str = (char*) malloc((entry->elemSize << 1) + 1);
        char hexa[50] = "";
        int i = 0;
        str[0] = 0;
        for(i = entry->elemSize - 1 ; i >= 0 ; i--)
        {
            sprintf(hexa, "%0.2x", *(((char*)(entry->value)) + i));
            strcat(str, hexa);
        }
        printf("HashMapEntry value for \"%s\" key is : {0x%s}\n", entry->key, str);
        free(str);
    }
    else
    {
        printf("HashMapEntry value is : {NULL}\n");
    }
}

HashMapEntry* CL_append(HashMapEntry* list, char* key, void* value, unsigned int elemSize)
{
    HashMapEntry *head = CL_create(key, value, elemSize);

    head->next = list;

    return head;
}

HashMapEntry* CL_create(char* key, void *value, unsigned int elemSize)
{
    HashMapEntry* list = (HashMapEntry*) malloc(sizeof(HashMapEntry));
    void* elem = malloc(elemSize);

    memcpy(elem, value, elemSize);

    list->key = (char*) malloc(strlen(key) * sizeof(char));
    strcpy(list->key, key);
    list->value = elem;
    list->elemSize = elemSize;
    list->next = NULL;

    return list;
}

int hashCode(char *str)
{
    int hash = 0;
    while(str != NULL && *str != 0)
    {
        hash+=*str;
        str++;
    }

    return hash;
}
