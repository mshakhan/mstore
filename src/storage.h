#ifndef STORAGE_H
#define STORAGE_H

#include "hashtable.h"

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) \
  +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

typedef hashtable_t mst_storage_t;

mst_storage_t*  mst_storage_create();
void            mst_storage_destroy(mst_storage_t* mst_storage);

int             mst_storage_set(mst_storage_t* mst_storage, char* key, char* value);
char*           mst_storage_get(mst_storage_t* mst_storage, const char* key);

#endif
