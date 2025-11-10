#ifndef _HASHTABLE_H
#define _HASHTABLE_H

/* File: hashtable.h
  */

/* Type: HashTable
  */
using HashTable = struct HashImplementation *;


/* TableHashFn
  */
using TableHashFn = int(*)(const void *elem, int numBuckets);


/* TableCompareFn
  */
using TableCompareFn = int(*)(const void *elem1, const void *elem2);
/* TableMapFn
 */
using TableMapFn = void(*)(void *elem, void *clientData);

/* TableMapFn2
 */
using TableMapFn2 = int(*)(void *elem, void *clientData);


/* TableElementFreeFn
  */
using TableElementFreeFn = void(*)(void *elem);

#ifdef __cplusplus
extern "C"
{
#endif

/* TableNew
  */

HashTable TableNew(int elemSize, int nBuckets,
                   TableHashFn hashFn, TableCompareFn compFn,
                   TableElementFreeFn freeFn);

HashTable TableNew2(int elemSize, int nBuckets, int nChains,
                    TableHashFn hashFn, TableCompareFn compFn,
                    TableElementFreeFn freeFn);


/* TableFree
 */
void TableFree(HashTable table);


/* TableCount
  */
int TableCount(HashTable table);


/* TableEnter
  */
void TableEnter(HashTable table, const void *newElem);

/* TableRemove
  */
int TableRemove(HashTable table, const void *delElem);


/* TableLookup
  */
void *TableLookup(HashTable table, const void *elemKey);


/* TableMap
  */
void TableMap(HashTable table, TableMapFn fn, void *clientData);

/* TableMapSafe
  */
void TableMapSafe(HashTable table, TableMapFn fn, void *clientData);

/* TableMap2
  */
void *TableMap2(HashTable table, TableMapFn2 fn, void *clientData);

/* TableMapSafe2
  */
void *TableMapSafe2(HashTable table, TableMapFn2 fn, void *clientData);

/* TableClear
  */
void TableClear(HashTable table);

#ifdef __cplusplus
}
#endif

#endif // _HASHTABLE_H