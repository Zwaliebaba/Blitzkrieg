#ifndef _DARRAY_H
#define _DARRAY_H

/* File: darray.h
  */

#ifdef __cplusplus
extern "C"
{
#endif

/* Type: DArray
  */
using DArray = struct DArrayImplementation *;


/* ArrayCompareFn
  */
using ArrayCompareFn = int(*)(const void *elem1, const void *elem2);


/* ArrayMapFn
  */
using ArrayMapFn = void(*)(void *elem, void *clientData);

/* ArrayMapFn2
  */
using ArrayMapFn2 = int(*)(void *elem, void *clientData);

/* ArrayElementFreeFn
 */
using ArrayElementFreeFn = void(*)(void *elem);


/* ArrayNew
  */
DArray ArrayNew(int elemSize, int numElemsToAllocate,
                ArrayElementFreeFn elemFreeFn);


/* ArrayFree
 */
void ArrayFree(DArray array);


/* ArrayLength
  */
int ArrayLength(DArray array);


/* ArrayNth
  */
void *ArrayNth(DArray array, int n);


/* ArrayAppend
  */
void ArrayAppend(DArray array, const void *newElem);

/* ArrayInsertAt
  */
void ArrayInsertAt(DArray array, const void *newElem, int n);

/* ArrayInsertSorted
  */
void ArrayInsertSorted(DArray array, const void *newElem, ArrayCompareFn comparator);

/* ArrayDeleteAt
 */
void ArrayDeleteAt(DArray array, int n);

/* ArrayDeleteAt
 */
void ArrayRemoveAt(DArray array, int n);

/* ArrayReplaceAt
  */
void ArrayReplaceAt(DArray array, const void *newElem, int n);


/* ArraySort
  */
void ArraySort(DArray array, ArrayCompareFn comparator);


#define NOT_FOUND -1	// returned when a search fails to find the key

/* ArraySearch
  */
int ArraySearch(DArray array, const void *key, ArrayCompareFn comparator,
                int fromIndex, int isSorted);


/* ArrayMap
  */
void ArrayMap(DArray array, ArrayMapFn fn, void *clientData);

/* ArrayMapBackwards
  */
void ArrayMapBackwards(DArray array, ArrayMapFn fn, void *clientData);

/* ArrayMap2
  */
void *ArrayMap2(DArray array, ArrayMapFn2 fn, void *clientData);

/* ArrayMapBackwards2
  */
void *ArrayMapBackwards2(DArray array, ArrayMapFn2 fn, void *clientData);

/* ArrayClear
  */
void ArrayClear(DArray array);

#ifdef __cplusplus
}
#endif

#endif // _DARRAY_