#ifndef MAP_H

    #define MAP_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    typedef struct HashMapEntry
    {
        char *key;
        void* value;
        unsigned int elemSize;
        struct HashMapEntry *next;
    } HashMapEntry;

    typedef struct HashMap
    {
        HashMapEntry **entries;
        unsigned int slots;
        unsigned int size;
        double growFactor;
        double loadFactor;
    } HashMap;

    int hashCode(char *str);

    HashMapEntry* CL_append(HashMapEntry* list, char* key, void* value, unsigned int elemSize);
    HashMapEntry* CL_create(char* key, void *value, unsigned int elemSize);
    HashMapEntry* HashMap_get(HashMap* map, char* key);
    void HashMapEntry_update(HashMapEntry* entry, void* value, unsigned int elemSize);
    void HashMapEntry_print(HashMapEntry* entry);
    void HashMapEntry_freeList(HashMapEntry** entry);

    HashMap* HashMap_create(unsigned int slots, double loadFactor, double growFactor);
    void HashMap_put(HashMap* map, char* key, void* value, unsigned int elemSize);
    HashMap* HashMap_extend(HashMap* map);
    void HashMap_free(HashMap** map);
    void HashMap_remove(HashMap* map, char* key);

#endif // MAP_H
