/* ************
 */
/* *****
 */
// todo: max results and ordering

#ifndef _GOACENG_H
#define _GOACENG_H

#ifdef __cplusplus
extern "C"
{

#endif


#define ENGINE_VERSION "1.8"
#define SERVER_TIMEOUT 3000
/* GServerList and GServer are the abstracted serverlist and server data types.
 */
typedef struct GServerListImplementation *GServerList;
typedef struct GServerImplementation *GServer;

/* A simple error type that we can use for return values ​​to certain functions */
typedef int GError;// we will define some error return values

typedef int gbool;// a simple boolean type

/* Various Server List States 
 */
typedef enum { sl_idle, sl_listxfer, sl_lanlist, sl_querying } GServerListState;

/* Comparison types for the ServerListSort function
 */
typedef enum { cm_int, cm_float, cm_strcase, cm_stricase } GCompareMode;


typedef enum { qt_basic, qt_info, qt_rules, qt_players, qt_info_rules, qt_status, qt_grouprooms, qt_masterinfo } GQueryType;

/* Messages that are passed to the ListCallBackFn */
#define	LIST_STATECHANGED		1 // ServerListState changed, no parameters
#define LIST_PROGRESS			2 // New Server updated, param1 = GServer (server updated), param2 = percent done

// Single callback function into the client app for status / progress messages
typedef void (*ListCallBackFn)(GServerList serverlist, int msg, void *instance, void *param1, void *param2);

// Callback function used for enumerating the keys/values ​​for a server
typedef void (*KeyEnumFn)(char *key, char *value, void *instance);


/* Callback parameter types (e.g. window handle, thread window, event, function etc) */
#define GCALLBACK_FUNCTION 1	// only currently supported callback type

#define GE_NOERROR		0
#define GE_NOSOCKET		1
#define GE_NODNS		2
#define GE_NOCONNECT	3
#define GE_BUSY			4
#define GE_DATAERROR	5

/* The hostname of the master server.
 */
extern char ServerListHostname[64];

/* ********
 */

/* ServerListNew
 */
GServerList ServerListNew(const char *gamename, const char *enginename, const char *seckey, int maxconcupdates, void *CallBackFn, int CallBackFnType, void *instance);

/* ServerListFree
 */
void ServerListFree(GServerList serverlist);

/* ServerListUpdate
 */
GError ServerListUpdate(GServerList serverlist, gbool async);


/* ServerListUpdate2
 */
GError ServerListUpdate2(GServerList serverlist, gbool async, char *filter, GQueryType querytype);


/* ServerListLANUpdate
 */// TODO: add filter
GError ServerListLANUpdate(GServerList serverlist, gbool async, int startsearchport, int endsearchport, int searchdelta);

/* ServerListAuxUpdate
 */
GError ServerListAuxUpdate(GServerList serverlist, char *ip, int port, gbool async, GQueryType querytype);

/* ServerListRemoveServer
 */
void ServerListRemoveServer(GServerList serverlist, char *ip, int port);

/* ServerListThink
 */
GError ServerListThink(GServerList serverlist);

/* ServerListHalt
 */
GError ServerListHalt(GServerList serverlist);

/* ServerListClear
 */
GError ServerListClear(GServerList serverlist);

/* ServerListState
 */
GServerListState ServerListState(GServerList serverlist);

/* ServerListErrorDesc
 */
char *ServerListErrorDesc(GServerList serverlist, GError error);

/* ServerListGetServer
 */
GServer ServerListGetServer(GServerList serverlist, int index);

/* ServerListCount
 */
int ServerListCount(GServerList serverlist);

/* ServerListSort
 */
void ServerListSort(GServerList serverlist, gbool ascending, char *sortkey, GCompareMode comparemode);


/* *************
 */

/* ServerGetPing
 */
int ServerGetPing(GServer server);

/* ServerGetAddress
 */
char *ServerGetAddress(GServer server);

/* ServerGetInetAddress
 */
unsigned int ServerGetInetAddress(GServer server);


/* ServerGetPort
 */
int ServerGetQueryPort(GServer server);

/* ServerGet[]Value
 */
char *ServerGetStringValue(GServer server, char *key, char *sdefault);
int ServerGetIntValue(GServer server, char *key, int idefault);
double ServerGetFloatValue(GServer server, char *key, double fdefault);
gbool ServerGetBoolValue(GServer server, char *key, gbool bdefault);


/* ServerGetPlayer[]Value
 */
char *ServerGetPlayerStringValue(GServer server, int playernum, char *key, char *sdefault);
int ServerGetPlayerIntValue(GServer server, int playernum, char *key, int idefault);
double ServerGetPlayerFloatValue(GServer server, int playernum, char *key, double fdefault);

/* ServerEnumKeys 
 */
void ServerEnumKeys(GServer server, KeyEnumFn KeyFn, void *instance);

#ifdef __cplusplus
}
#endif

#endif // _GOACENG_H