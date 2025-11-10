/* MD5.H - header file for MD5C.C */

/* Copyright (C) 1991, RSA Data Security, Inc.  */

/* GLOBAL.H - RSAREF types and constants */

/* PROTOTYPES should be set to one if and only if the compiler supports
   */

#ifdef __cplusplus
extern "C"
{

#endif

#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

/* MD5 context. */
typedef struct
{
  UINT4 state[4];/* state (ABCD) */
  UINT4 count[2];/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer */
} MD5_CTX;

void MD5Init PROTO_LIST((MD5_CTX *));
void MD5Update PROTO_LIST((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final PROTO_LIST((unsigned char [16], MD5_CTX *));
void MD5Print PROTO_LIST((unsigned char [16], char[33]));
void MD5Digest PROTO_LIST((unsigned char *, unsigned int, char[33]));
#ifdef __cplusplus
}
#endif