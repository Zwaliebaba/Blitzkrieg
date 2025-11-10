/* GameSpy Peer SDK 
 */

#ifndef _PEERCENGINE_H_
#define _PEERCENGINE_H_

#ifdef __cplusplus
extern "C"
{

#endif

/* ************
 */
#include "peerMain.h"

/* *************
 */
PEERBool piCEngineInit
(
    PEER peer
    );

void piCEngineCleanup
(
    PEER peer
    );

PEERBool piCEngineStartListingGames
(
    PEER peer,
    const char *filter
    );

void piCEngineStopListingGames
(
    PEER peer
    );

void piListingGamesChannelMessage
(
    CHAT chat,
    const char *channel,
    const char *user,
    const char *message,
    int type,
    PEER peer
    );

void piListingGamesChannelKicked
(
    CHAT chat,
    const char *channel,
    const char *user,
    const char *reason,
    PEER peer
    );

PEERBool piCEngineStartListingGroups
(
    PEER peer
    );

void piCEngineStopListingGroups
(
    PEER peer
    );

void piCEngineThink
(
    PEER peer
    );

#ifdef __cplusplus
}
#endif

#endif