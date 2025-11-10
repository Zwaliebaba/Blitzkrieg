/* GameSpy Peer SDK 
 */

#ifndef _PEERMANGLE_H_
#define _PEERMANGLE_H_

#ifdef __cplusplus
extern "C"
{

#endif

/* ************
 */
#include "peer.h"
#include "peerMain.h"

/* ************
 */
#define PI_USER_MAX_LEN        128

/* *************
 */
void piMangleTitleRoom
(
    char buffer[PI_ROOM_MAX_LEN],
    const char *title
    );

void piMangleGroupRoom
(
    char buffer[PI_ROOM_MAX_LEN],
    int groupID
    );

void piMangleStagingRoom
(
    char buffer[PI_ROOM_MAX_LEN],
    const char *title,
    unsigned int IP
    );

void piMangleUpdatesRoom
(
    char buffer[PI_ROOM_MAX_LEN],
    const char *title,
    int groupID
    );

void piMangleUser
(
    char buffer[PI_USER_MAX_LEN],
    unsigned int IP,
    int profileID
    );

PEERBool piDemangleUser
(
    const char buffer[PI_USER_MAX_LEN],
    unsigned int *IP,
    int *profileID
    );

void piMangleIP
(
    char buffer[11],
    unsigned int IP
    );

unsigned int piDemangleIP
(
    const char buffer[11]
    );

#ifdef __cplusplus
}
#endif

#endif