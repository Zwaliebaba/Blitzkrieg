/* GameSpy Peer SDK 
 */

#ifndef _PEERROOMS_H_
#define _PEERROOMS_H_

#ifdef __cplusplus
extern "C"
{

#endif

/* ************
 */
#include "peerMain.h"

/* *************
 */
PEERBool piRoomsInit
(
    PEER peer
    );

void piRoomsCleanup
(
    PEER peer
    );

void piStartedEnteringRoom
(
    PEER peer,
    RoomType roomType,
    const char *room
    );

void piFinishedEnteringRoom
(
    PEER peer,
    RoomType roomType,
    const char *name
    );

PEERBool piStartHosting
(
    PEER peer,
    SOCKET socket
    );

void piResetHosting
(
    PEER peer
    );

void piStopHosting
(
    PEER peer
    );

PEERBool piStartReporting
(
    PEER peer,
    SOCKET socket
    );

void piStopReporting
(
    PEER peer
    );

void piLeaveRoom
(
    PEER peer,
    RoomType roomType,
    const char *reason
    );

PEERBool piRoomToType
(
    PEER peer,
    const char *room,
    RoomType *roomType
    );

void piSetLocalFlags
(
    PEER peer
    );

#ifdef __cplusplus
}
#endif

#endif