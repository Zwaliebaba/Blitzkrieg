/* *****
 */

#ifndef _GQUERYREPORTING_H_
#define _GQUERYREPORTING_H_

#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__KATANA__) && !defined(__mips64)
#include "::nonport.h"
#else
#include "../nonport.h"
#endif


#ifdef __cplusplus
extern "C"
{

#endif


/* *******
 */

#define E_GOA_WSOCKERROR	1
#define E_GOA_BINDERROR		2
#define E_GOA_DNSERROR		3
#define E_GOA_CONNERROR		4
/* ********
 */
#define NUM_PORTS_TO_TRY 100

/* The hostname of the master server.
 */
extern char qr_hostname[64];

/* *******
 */
typedef void (*qr_querycallback_t)(char *outbuf, int maxlen, void *userdata);

/* **********
 */
typedef struct qr_implementation_s *qr_t;


/* ************
 */
int qr_init(/* [out] */qr_t *qrec, const char *ip, int *pnBaseport, const char *gamename, const char *secret_key,
                       qr_querycallback_t qr_basic_callback,
                       qr_querycallback_t qr_info_callback,
                       qr_querycallback_t qr_rules_callback,
                       qr_querycallback_t qr_players_callback,
                       void *userdata);

/* ************
 */
int qr_init_socket(/* [out] */qr_t *qrec, SOCKET s, const char *gamename, const char *secret_key,
                              qr_querycallback_t qr_basic_callback,
                              qr_querycallback_t qr_info_callback,
                              qr_querycallback_t qr_rules_callback,
                              qr_querycallback_t qr_players_callback,
                              void *userdata);


/* ******************
 */
void qr_process_queries(qr_t qrec);
void qr_process_queries_no_heartbeat(qr_t qrec);

/* ******************
 */
void qr_parse_query(qr_t qrec, char *query, int len, struct sockaddr *sender);

/* ****************
 */
void qr_send_statechanged(qr_t qrec);

/* ****************
 */
void qr_send_exiting(qr_t qrec);


/* ****************
 */
void qr_shutdown(qr_t qrec);

void qr_check_queries(qr_t qrec);


/* for CDKey SDK integration */
typedef void (*cdkey_process_t)(char *buf, int len, struct sockaddr *fromaddr);

struct qr_implementation_s
{
  SOCKET querysock;
  SOCKET hbsock;
  char gamename[64];
  char secret_key[128];
  qr_querycallback_t qr_basic_callback;
  qr_querycallback_t qr_info_callback;
  qr_querycallback_t qr_rules_callback;
  qr_querycallback_t qr_players_callback;
  unsigned long lastheartbeat;
  int queryid;
  int packetnumber;
  int qport;
  char no_query;
  struct sockaddr_in hbaddr;
  cdkey_process_t cdkeyprocess;
  void *udata;
};


#ifdef __cplusplus
}
#endif

#endif