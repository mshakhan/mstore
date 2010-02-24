#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//#include "hashtable.h"
#include "storage.h"

unsigned int mst_storage_key_hash_fn(const void * key) {
    char* data = (char*)key;
    uint32_t len = strlen(key);
    uint32_t hash = len;
    uint32_t tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

int mst_storage_key_cmp_fn(const void* key1, const void* key2) {
  if (strcmp((char*)key1, (char*)key2) == 0) {
    return 1;
  } else {
    return 0;
  }
}

void mst_storage_free_fn(void* key) {
  free(key);
}

mst_storage_t* mst_storage_create() {
  hashtable_t* tbl = hashtable_create(
    mst_storage_key_hash_fn,
    mst_storage_key_cmp_fn,
    mst_storage_free_fn,
    mst_storage_free_fn
  );
  return (mst_storage_t*)tbl;
}

void mst_storage_destroy(mst_storage_t* mst_storage) {
  hashtable_destroy((hashtable_t*)mst_storage);
}

int mst_storage_set(mst_storage_t* mst_storage, char* key, char* value) {
  return hashtable_set((hashtable_t*)mst_storage, (void*)key, (void*)value);
}

char* mst_storage_get(mst_storage_t* mst_storage, const char* key) {
  return (char*)hashtable_get((hashtable_t*)mst_storage, (void*)key);
}

