/* GameSpy Peer SDK 
 */

#ifndef _PEERGLOBALCALLBACKS_H_
#define _PEERGLOBALCALLBACKS_H_

#ifdef __cplusplus
extern "C"
{

#endif

/* ************
 */
#include "peerMain.h"

/* ************
 */
#define PI_UTM_LAUNCH          "GML"
#define PI_UTM_XPING           "PNG"

/* *************
 */

/* Chat.
 */
void piChatDisconnected
(
    CHAT chat,
    const char *reason,
    PEER peer
    );

void piChatPrivateMessage
(
    CHAT chat,
    const char *user,
    const char *message,
    int type,
    PEER peer
    );

void piSetChannelCallbacks
(
    PEER peer,
    chatChannelCallbacks *channelCallbacks
    );

/* GOA.
 */
void piGOABasicCallback
(
    char *outbuf,
    int maxlen,
    PEER peer
    );

void piGOAInfoCallback
(
    char *outbuf,
    int maxlen,
    PEER peer
    );

void piGOARulesCallback
(
    char *outbuf,
    int maxlen,
    PEER peer
    );

void piGOAPlayersCallback
(
    char *outbuf,
    int maxlen,
    PEER peer
    );

#ifdef __cplusplus
}
#endif

#endif