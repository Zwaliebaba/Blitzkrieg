/*
GameSpy Chat SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2001 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
*/

/*************
** INCLUDES **
*************/
#include <stdlib.h>
#include <string.h>
#include "chatCallbacks.h"
#include "chatMain.h"
#include "chatChannel.h"
#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__KATANA__)  && !defined(__mips64)
#include "::darray.h"
#include "::nonport.h"
#else
#include "../darray.h"
#include "../nonport.h"
#endif

/************
** DEFINES **
************/
#define ASSERT_DATA(data)   assert(data != NULL); assert(data->type >= 0); assert(data->type < CALLBACK_NUM); assert(data->callback != NULL); assert(data->callbackParams != NULL);  assert(data->ID >= 0);
#define RAW                 callbackParams->raw
#define REASON              callbackParams->reason
#define USER                callbackParams->user
#define MESSAGE             callbackParams->message
#define TYPE                callbackParams->type
#define CHANNEL             callbackParams->channel
#define KICKER              callbackParams->kicker
#define KICKEE              callbackParams->kickee
#define TOPIC               callbackParams->topic
#define MODE                callbackParams->mode
#define SUCCESS             callbackParams->success
#define INDEX               callbackParams->index
#define NUM_USERS           callbackParams->numUsers
#define NUM_CHANNELS        callbackParams->numChannels
#define CHANNELS            callbackParams->channels
#define TOPICS              callbackParams->topics
#define ENABLED             callbackParams->enabled
#define PASSWORD            callbackParams->password
#define USERS               callbackParams->users
#define MODES               callbackParams->modes
#define ADDRESS             callbackParams->address
#define WHY                 callbackParams->why
#define OLD_NICK            callbackParams->oldNick
#define NEW_NICK            callbackParams->newNick
#define NUM_BANS            callbackParams->numBans
#define BANS                callbackParams->bans
#define NICK                callbackParams->nick
#define NAME                callbackParams->name
#define NUM                 callbackParams->num
#define KEY                 callbackParams->key
#define KEYS                callbackParams->keys
#define VALUE               callbackParams->value
#define VALUES              callbackParams->values
#define RESULT              callbackParams->result
#define COPY(param)         if(srcParams->param != NULL)\
							{\
								destParams->param = _strdup(srcParams->param);\
								if(destParams->param == NULL)\
								{\
									gsifree(destParams);\
									gsifree(data.channel);\
					/*ERRCON*/		return CHATFalse;\
								}\
							}
#define COPY_MODE()         if(srcParams->mode != NULL)\
							{\
								destParams->mode = (CHATChannelMode *)gsimalloc(sizeof(CHATChannelMode));\
								if(destParams->mode == NULL)\
								{\
									gsifree(destParams);\
									gsifree(data.channel);\
					/*ERRCON*/		return CHATFalse;\
								}\
								memcpy(destParams->mode, srcParams->mode, sizeof(CHATChannelMode));\
							}
#define COPY_STR_ARRAY(array, num)  assert(srcParams->num >= 0);\
									if(!srcParams->array)\
										destParams->array = NULL;\
									else\
									{\
										destParams->array = (char **)gsimalloc(sizeof(char *) * srcParams->num);\
										if(destParams->array == NULL)\
										{\
											gsifree(destParams);\
											gsifree(data.channel);\
							/*ERRCON*/		return CHATFalse;\
										}\
										for(i = 0 ; i < srcParams->num ; i++)\
										{\
											if(srcParams->array[i] == NULL)\
												destParams->array[i] = NULL;\
											else\
											{\
												destParams->array[i] = _strdup(srcParams->array[i]);\
												if(destParams->array[i] == NULL)\
												{\
													for(i-- ; i >= 0 ; i--)\
														gsifree(destParams->array[i]);\
													gsifree(destParams->array);\
													gsifree(destParams);\
													gsifree(data.channel);\
													return CHATFalse;\
												}\
											}\
										}\
									}
#define COPY_INT_ARRAY(array, num)  assert(srcParams->num >= 0);\
                                    if(srcParams->num > 0)\
									{\
										assert(srcParams->array != NULL);\
										len = (sizeof(int) * srcParams->num);\
										destParams->array = (int *)gsimalloc(len);\
										if(destParams->array == NULL)\
										{\
											gsifree(destParams);\
											gsifree(data.channel);\
							/*ERRCON*/		return CHATFalse;\
										}\
										memcpy(destParams->array, srcParams->array, len);\
									}

/**********
** TYPES **
**********/
typedef struct ciCallbackData
{
  int type;
  void *callback;
  void *callbackParams;
  void *param;
  int ID;
  char *channel;
} ciCallbackData;

/**************
** FUNCTIONS **
**************/
static void ciCallbacksArrayElementFreeFn(void *elem)
{
  ciCallbackData *data = elem;
  assert(data != NULL);

  gsifree(data->channel);
}

static void ciFreeCallbackData(ciCallbackData *data)
{
  ASSERT_DATA(data);

  // Find which type of callback it is.
  /////////////////////////////////////
  switch (data->type)
  {
    case CALLBACK_RAW:
    {
      ciCallbackRawParams *callbackParams = data->callbackParams;
      gsifree(RAW);
      break;
    }

    case CALLBACK_DISCONNECTED:
    {
      ciCallbackDisconnectedParams *callbackParams = data->callbackParams;
      gsifree(REASON);
      break;
    }

    case CALLBACK_PRIVATE_MESSAGE:
    {
      ciCallbackPrivateMessageParams *callbackParams = data->callbackParams;
      gsifree(USER);
      gsifree(MESSAGE);
      break;
    }

    case CALLBACK_INVITED:
    {
      ciCallbackInvitedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      break;
    }

    case CALLBACK_CHANNEL_MESSAGE:
    {
      ciCallbackChannelMessageParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      gsifree(MESSAGE);
      break;
    }

    case CALLBACK_KICKED:
    {
      ciCallbackKickedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      gsifree(REASON);
      break;
    }

    case CALLBACK_USER_JOINED:
    {
      ciCallbackUserJoinedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      break;
    }

    case CALLBACK_USER_PARTED:
    {
      ciCallbackUserPartedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      gsifree(REASON);
      gsifree(KICKER);
      break;
    }

    case CALLBACK_USER_CHANGED_NICK:
    {
      ciCallbackUserChangedNickParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(OLD_NICK);
      gsifree(NEW_NICK);
      break;
    }

    case CALLBACK_TOPIC_CHANGED:
    {
      ciCallbackTopicChangedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(TOPIC);
      break;
    }

    case CALLBACK_CHANNEL_MODE_CHANGED:
    {
      ciCallbackChannelModeChangedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(MODE);
      break;
    }

    case CALLBACK_USER_MODE_CHANGED:
    {
      ciCallbackUserModeChangedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      break;
    }

    case CALLBACK_USER_LIST_UPDATED:
    {
      ciCallbackUserListUpdatedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      break;
    }

    case CALLBACK_CONNECT:
    {
      ciCallbackConnectParams *callbackParams = data->callbackParams;
      break;
    }

    case CALLBACK_ENUM_CHANNELS_EACH:
    {
      ciCallbackEnumChannelsEachParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(TOPIC);
      break;
    }

    case CALLBACK_ENUM_CHANNELS_ALL:
    {
      int i;
      ciCallbackEnumChannelsAllParams *callbackParams = data->callbackParams;
      for (i = 0; i < NUM_CHANNELS; i++)
      {
        gsifree(CHANNELS[i]);
        gsifree(TOPICS[i]);
      }
      gsifree(CHANNELS);
      gsifree(TOPICS);
      gsifree(NUM_USERS);
      break;
    }

    case CALLBACK_ENTER_CHANNEL:
    {
      ciCallbackEnterChannelParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      break;
    }

    case CALLBACK_GET_CHANNEL_TOPIC:
    {
      ciCallbackGetChannelTopicParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(TOPIC);
      break;
    }

    case CALLBACK_GET_CHANNEL_MODE:
    {
      ciCallbackGetChannelModeParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(MODE);
      break;
    }

    case CALLBACK_GET_CHANNEL_PASSWORD:
    {
      ciCallbackGetChannelPasswordParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(PASSWORD);
      break;
    }

    case CALLBACK_ENUM_USERS:
    {
      int i;
      ciCallbackEnumUsersParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      for (i = 0; i < NUM_USERS; i++)
        gsifree(USERS[i]);
      gsifree(USERS);
      gsifree(MODES);
      break;
    }

    case CALLBACK_GET_USER_INFO:
    {
      int i;
      ciCallbackGetUserInfoParams *callbackParams = data->callbackParams;
      gsifree(NICK);
      gsifree(USER);
      gsifree(NAME);
      gsifree(ADDRESS);
      for (i = 0; i < NUM_CHANNELS; i++)
        gsifree(CHANNELS[i]);
      gsifree(CHANNELS);
      break;
    }

    case CALLBACK_GET_BASIC_USER_INFO:
    {
      ciCallbackGetBasicUserInfoParams *callbackParams = data->callbackParams;
      gsifree(NICK);
      gsifree(USER);
      gsifree(ADDRESS);
      break;
    }

    case CALLBACK_GET_CHANNEL_BASIC_USER_INFO:
    {
      ciCallbackGetChannelBasicUserInfoParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(NICK);
      gsifree(USER);
      gsifree(ADDRESS);
      break;
    }

    case CALLBACK_GET_USER_MODE:
    {
      ciCallbackGetUserModeParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      break;
    }

    case CALLBACK_ENUM_CHANNEL_BANS:
    {
      int i;
      ciCallbackEnumChannelBansParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      for (i = 0; i < NUM_BANS; i++)
        gsifree(BANS[i]);
      gsifree(BANS);
      break;
    }

    case CALLBACK_NICK_ERROR:
    {
      ciCallbackNickErrorParams *callbackParams = data->callbackParams;
      gsifree(NICK);
      break;
    }

    case CALLBACK_CHANGE_NICK:
    {
      ciCallbackChangeNickParams *callbackParams = data->callbackParams;
      gsifree(OLD_NICK);
      gsifree(NEW_NICK);
      break;
    }

    case CALLBACK_NEW_USER_LIST:
    {
      int i;
      ciCallbackNewUserListParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      for (i = 0; i < NUM_USERS; i++)
        gsifree(USERS[i]);
      gsifree(USERS);
      gsifree(MODES);
      break;
    }

    case CALLBACK_BROADCAST_KEY_CHANGED:
    {
      ciCallbackBroadcastKeyChangedParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      gsifree(KEY);
      gsifree(VALUE);
      break;
    }

    case CALLBACK_GET_GLOBAL_KEYS:
    {
      int i;
      ciCallbackGetGlobalKeysParams *callbackParams = data->callbackParams;
      gsifree(USER);
      for (i = 0; i < NUM; i++)
      {
        gsifree(KEYS[i]);
        if (VALUES)
          gsifree(VALUES[i]);
      }
      gsifree(KEYS);
      gsifree(VALUES);
      break;
    }

    case CALLBACK_GET_CHANNEL_KEYS:
    {
      int i;
      ciCallbackGetChannelKeysParams *callbackParams = data->callbackParams;
      gsifree(CHANNEL);
      gsifree(USER);
      for (i = 0; i < NUM; i++)
      {
        gsifree(KEYS[i]);
        if (VALUES)
          gsifree(VALUES[i]);
      }
      gsifree(KEYS);
      gsifree(VALUES);
      break;
    }

    default:
      // The type for this callback is messed up.
      ///////////////////////////////////////////
      assert(0);
  }

  // gsifree the params structure.
  /////////////////////////////
  gsifree(data->callbackParams);
}

CHATBool ciInitCallbacks(ciConnection *connection)
{
  // Setup the darray.
  ////////////////////
  connection->callbackList = ArrayNew(sizeof(ciCallbackData), 128, ciCallbacksArrayElementFreeFn);
  if (connection->callbackList == NULL) return CHATFalse;

  return CHATTrue;
}

void ciCleanupCallbacks(CHAT chat)
{
  CONNECTION;

  // Cleanup.
  ///////////
  if (connection->callbackList != NULL)
  {
    ciCallbackData *data;
    int len;
    int i;

    // Get the number of callbacks.
    ///////////////////////////////
    len = ArrayLength(connection->callbackList);

    // gsifree the data.
    /////////////////
    for (i = 0; i < len; i++)
    {
      data = (ciCallbackData *) ArrayNth(connection->callbackList, i);
      ASSERT_DATA(data);

      // gsifree the data.
      /////////////////
      ciFreeCallbackData(data);
    }

    // gsifree the list.
    /////////////////
    ArrayFree(connection->callbackList);
  }
}

CHATBool ciAddCallback_(CHAT chat, int type, void *callback, void *callbackParams, void *param, int ID, const char *channel, size_t callbackParamsSize)
{
  ciCallbackData data;
  int len;
  int i;
  CONNECTION;

  assert(type >= 0);
  assert(type < CALLBACK_NUM);
  assert(connection->callbackList != NULL);
  assert(callback != NULL);
  assert(callbackParams != NULL);
  assert(callbackParamsSize > 0);
  assert(ID >= 0);
#ifdef _DEBUG
  if (channel != NULL)
    assert(channel[0] != '\0');
#endif

  // Setup the data.
  //////////////////
  memset(&data, 0, sizeof(ciCallbackData));
  data.type = type;
  data.callback = callback;
#ifndef _DEBUG
  data.callbackParams = gsimalloc(callbackParamsSize);
#else
  data.callbackParams = gsimalloc(callbackParamsSize + 64);
  memset(data.callbackParams, 0xC4, callbackParamsSize + 64);
#endif
  if (data.callbackParams == NULL) return CHATFalse;//ERRCON
  memcpy(data.callbackParams, callbackParams, callbackParamsSize);
  data.param = param;
  data.ID = ID;
  if (channel == NULL) data.channel = NULL;
  else
  {
    len = (strlen(channel) + 1);
    data.channel = (char *) gsimalloc(len);
    if (data.channel == NULL)
    {
      gsifree(data.callbackParams);
      return CHATFalse;//ERRCON
    }
    memcpy(data.channel, channel, len);
  }

  // Find which type of callback it is.
  /////////////////////////////////////
  switch (data.type)
  {
    case CALLBACK_RAW:
    {
      ciCallbackRawParams *destParams = data.callbackParams;
      ciCallbackRawParams *srcParams = callbackParams;
      COPY(raw);
      break;
    }

    case CALLBACK_DISCONNECTED:
    {
      ciCallbackDisconnectedParams *destParams = data.callbackParams;
      ciCallbackDisconnectedParams *srcParams = callbackParams;
      COPY(reason);
      break;
    }

    case CALLBACK_PRIVATE_MESSAGE:
    {
      ciCallbackPrivateMessageParams *destParams = data.callbackParams;
      ciCallbackPrivateMessageParams *srcParams = callbackParams;
      COPY(user);
      COPY(message);
      break;
    }

    case CALLBACK_INVITED:
    {
      ciCallbackInvitedParams *destParams = data.callbackParams;
      ciCallbackInvitedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      break;
    }

    case CALLBACK_CHANNEL_MESSAGE:
    {
      ciCallbackChannelMessageParams *destParams = data.callbackParams;
      ciCallbackChannelMessageParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      COPY(message);
      break;
    }

    case CALLBACK_KICKED:
    {
      ciCallbackKickedParams *destParams = data.callbackParams;
      ciCallbackKickedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      COPY(reason);
      break;
    }

    case CALLBACK_USER_JOINED:
    {
      ciCallbackUserJoinedParams *destParams = data.callbackParams;
      ciCallbackUserJoinedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      break;
    }

    case CALLBACK_USER_PARTED:
    {
      ciCallbackUserPartedParams *destParams = data.callbackParams;
      ciCallbackUserPartedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      COPY(reason);
      COPY(kicker);
      break;
    }

    case CALLBACK_USER_CHANGED_NICK:
    {
      ciCallbackUserChangedNickParams *destParams = data.callbackParams;
      ciCallbackUserChangedNickParams *srcParams = callbackParams;
      COPY(channel);
      COPY(oldNick);
      COPY(newNick);
      break;
    }

    case CALLBACK_TOPIC_CHANGED:
    {
      ciCallbackTopicChangedParams *destParams = data.callbackParams;
      ciCallbackTopicChangedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(topic);
      break;
    }

    case CALLBACK_CHANNEL_MODE_CHANGED:
    {
      ciCallbackChannelModeChangedParams *destParams = data.callbackParams;
      ciCallbackChannelModeChangedParams *srcParams = callbackParams;
      COPY(channel);
      COPY_MODE();
      break;
    }

    case CALLBACK_USER_MODE_CHANGED:
    {
      ciCallbackUserModeChangedParams *destParams = data.callbackParams;
      ciCallbackUserModeChangedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      break;
    }

    case CALLBACK_USER_LIST_UPDATED:
    {
      ciCallbackUserListUpdatedParams *destParams = data.callbackParams;
      ciCallbackUserListUpdatedParams *srcParams = callbackParams;
      COPY(channel);
      break;
    }

    case CALLBACK_CONNECT:
    {
      ciCallbackConnectParams *destParams = data.callbackParams;
      ciCallbackConnectParams *srcParams = callbackParams;
      break;
    }

    case CALLBACK_ENUM_CHANNELS_EACH:
    {
      ciCallbackEnumChannelsEachParams *destParams = data.callbackParams;
      ciCallbackEnumChannelsEachParams *srcParams = callbackParams;
      COPY(channel);
      COPY(topic);
      break;
    }

    case CALLBACK_ENUM_CHANNELS_ALL:
    {
      ciCallbackEnumChannelsAllParams *destParams = data.callbackParams;
      ciCallbackEnumChannelsAllParams *srcParams = callbackParams;
      COPY_STR_ARRAY(channels, numChannels);
      COPY_STR_ARRAY(topics, numChannels);
      COPY_INT_ARRAY(numUsers, numChannels);
      break;
    }

    case CALLBACK_ENTER_CHANNEL:
    {
      ciCallbackEnterChannelParams *destParams = data.callbackParams;
      ciCallbackEnterChannelParams *srcParams = callbackParams;
      COPY(channel);
      break;
    }

    case CALLBACK_GET_CHANNEL_TOPIC:
    {
      ciCallbackGetChannelTopicParams *destParams = data.callbackParams;
      ciCallbackGetChannelTopicParams *srcParams = callbackParams;
      COPY(channel);
      COPY(topic);
      break;
    }

    case CALLBACK_GET_CHANNEL_MODE:
    {
      ciCallbackGetChannelModeParams *destParams = data.callbackParams;
      ciCallbackGetChannelModeParams *srcParams = callbackParams;
      COPY(channel);
      COPY_MODE();
      break;
    }

    case CALLBACK_GET_CHANNEL_PASSWORD:
    {
      ciCallbackGetChannelPasswordParams *destParams = data.callbackParams;
      ciCallbackGetChannelPasswordParams *srcParams = callbackParams;
      COPY(channel);
      COPY(password);
      break;
    }

    case CALLBACK_ENUM_USERS:
    {
      ciCallbackEnumUsersParams *destParams = data.callbackParams;
      ciCallbackEnumUsersParams *srcParams = callbackParams;
      COPY(channel);
      COPY_STR_ARRAY(users, numUsers);
      COPY_INT_ARRAY(modes, numUsers);
      break;
    }

    case CALLBACK_GET_USER_INFO:
    {
      ciCallbackGetUserInfoParams *destParams = data.callbackParams;
      ciCallbackGetUserInfoParams *srcParams = callbackParams;
      COPY(nick);
      COPY(user);
      COPY(name);
      COPY(address);
      COPY_STR_ARRAY(channels, numChannels);
      break;
    }

    case CALLBACK_GET_BASIC_USER_INFO:
    {
      ciCallbackGetBasicUserInfoParams *destParams = data.callbackParams;
      ciCallbackGetBasicUserInfoParams *srcParams = callbackParams;
      COPY(nick);
      COPY(user);
      COPY(address);
      break;
    }

    case CALLBACK_GET_CHANNEL_BASIC_USER_INFO:
    {
      ciCallbackGetChannelBasicUserInfoParams *destParams = data.callbackParams;
      ciCallbackGetChannelBasicUserInfoParams *srcParams = callbackParams;
      COPY(channel);
      COPY(nick);
      COPY(user);
      COPY(address);
      break;
    }

    case CALLBACK_GET_USER_MODE:
    {
      ciCallbackGetUserModeParams *destParams = data.callbackParams;
      ciCallbackGetUserModeParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      break;
    }

    case CALLBACK_ENUM_CHANNEL_BANS:
    {
      ciCallbackEnumChannelBansParams *destParams = data.callbackParams;
      ciCallbackEnumChannelBansParams *srcParams = callbackParams;
      COPY(channel);
      COPY_STR_ARRAY(bans, numBans);
      break;
    }

    case CALLBACK_NICK_ERROR:
    {
      ciCallbackNickErrorParams *destParams = data.callbackParams;
      ciCallbackNickErrorParams *srcParams = callbackParams;
      COPY(nick);
      break;
    }

    case CALLBACK_CHANGE_NICK:
    {
      ciCallbackChangeNickParams *destParams = data.callbackParams;
      ciCallbackChangeNickParams *srcParams = callbackParams;
      COPY(oldNick);
      COPY(newNick);
      break;
    }

    case CALLBACK_NEW_USER_LIST:
    {
      ciCallbackNewUserListParams *destParams = data.callbackParams;
      ciCallbackNewUserListParams *srcParams = callbackParams;
      COPY(channel);
      COPY_STR_ARRAY(users, numUsers);
      COPY_INT_ARRAY(modes, numUsers);
      break;
    }

    case CALLBACK_BROADCAST_KEY_CHANGED:
    {
      ciCallbackBroadcastKeyChangedParams *destParams = data.callbackParams;
      ciCallbackBroadcastKeyChangedParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      COPY(key);
      COPY(value);
      break;
    }

    case CALLBACK_GET_GLOBAL_KEYS:
    {
      ciCallbackGetGlobalKeysParams *destParams = data.callbackParams;
      ciCallbackGetGlobalKeysParams *srcParams = callbackParams;
      COPY(user);
      COPY_STR_ARRAY(keys, num);
      COPY_STR_ARRAY(values, num);
      break;
    }

    case CALLBACK_GET_CHANNEL_KEYS:
    {
      ciCallbackGetChannelKeysParams *destParams = data.callbackParams;
      ciCallbackGetChannelKeysParams *srcParams = callbackParams;
      COPY(channel);
      COPY(user);
      COPY_STR_ARRAY(keys, num);
      COPY_STR_ARRAY(values, num);
      break;
    }

    default:
      // The type for this callback is messed up.
      ///////////////////////////////////////////
      assert(0);
  }

  // Add it to the array.
  ///////////////////////
  ArrayAppend(connection->callbackList, &data);

  return CHATTrue;
}

void ciCallCallback(CHAT chat, ciCallbackData *data)
{
  void *param;
  CONNECTION;

  ASSERT_DATA(data);

  // Cache the param.
  ///////////////////
  param = data->param;

  // Find which type of callback it is.
  /////////////////////////////////////
  switch (data->type)
  {
    case CALLBACK_RAW:
    {
      ciCallbackRawParams *callbackParams = data->callbackParams;
      chatRaw callback = data->callback;
      callback(chat, RAW, param);
      break;
    }

    case CALLBACK_DISCONNECTED:
    {
      ciCallbackDisconnectedParams *callbackParams = data->callbackParams;
      chatDisconnected callback = data->callback;
      callback(chat, REASON, param);
      break;
    }

    case CALLBACK_PRIVATE_MESSAGE:
    {
      ciCallbackPrivateMessageParams *callbackParams = data->callbackParams;
      chatPrivateMessage callback = data->callback;
      callback(chat, USER, MESSAGE, TYPE, param);
      break;
    }

    case CALLBACK_INVITED:
    {
      ciCallbackInvitedParams *callbackParams = data->callbackParams;
      chatInvited callback = data->callback;
      callback(chat, CHANNEL, USER, param);
      break;
    }

    case CALLBACK_CHANNEL_MESSAGE:
    {
      ciCallbackChannelMessageParams *callbackParams = data->callbackParams;
      chatChannelMessage callback = data->callback;
      callback(chat, CHANNEL, USER, MESSAGE, TYPE, param);
      break;
    }

    case CALLBACK_KICKED:
    {
      ciCallbackKickedParams *callbackParams = data->callbackParams;
      chatKicked callback = data->callback;
      callback(chat, CHANNEL, USER, REASON, param);
      break;
    }

    case CALLBACK_USER_JOINED:
    {
      ciCallbackUserJoinedParams *callbackParams = data->callbackParams;
      chatUserJoined callback = data->callback;
      callback(chat, CHANNEL, USER, MODE, param);
      break;
    }

    case CALLBACK_USER_PARTED:
    {
      ciCallbackUserPartedParams *callbackParams = data->callbackParams;
      chatUserParted callback = data->callback;
      callback(chat, CHANNEL, USER, WHY, REASON, KICKER, param);
      break;
    }

    case CALLBACK_USER_CHANGED_NICK:
    {
      ciCallbackUserChangedNickParams *callbackParams = data->callbackParams;
      chatUserChangedNick callback = data->callback;
      callback(chat, CHANNEL, OLD_NICK, NEW_NICK, param);
      break;
    }

    case CALLBACK_TOPIC_CHANGED:
    {
      ciCallbackTopicChangedParams *callbackParams = data->callbackParams;
      chatTopicChanged callback = data->callback;
      callback(chat, CHANNEL, TOPIC, param);
      break;
    }

    case CALLBACK_CHANNEL_MODE_CHANGED:
    {
      ciCallbackChannelModeChangedParams *callbackParams = data->callbackParams;
      chatChannelModeChanged callback = data->callback;
      callback(chat, CHANNEL, MODE, param);
      break;
    }

    case CALLBACK_USER_MODE_CHANGED:
    {
      ciCallbackUserModeChangedParams *callbackParams = data->callbackParams;
      chatUserModeChanged callback = data->callback;
      callback(chat, CHANNEL, USER, MODE, param);
      break;
    }

    case CALLBACK_USER_LIST_UPDATED:
    {
      ciCallbackUserListUpdatedParams *callbackParams = data->callbackParams;
      chatUserListUpdated callback = data->callback;
      callback(chat, CHANNEL, param);
      break;
    }

    case CALLBACK_CONNECT:
    {
      ciCallbackConnectParams *callbackParams = data->callbackParams;
      chatConnectCallback callback = data->callback;
      callback(chat, SUCCESS, param);
      break;
    }

    case CALLBACK_ENUM_CHANNELS_EACH:
    {
      ciCallbackEnumChannelsEachParams *callbackParams = data->callbackParams;
      chatEnumChannelsCallbackEach callback = data->callback;
      callback(chat, SUCCESS, INDEX, CHANNEL, TOPIC, NUM_USERS, param);
      break;
    }

    case CALLBACK_ENUM_CHANNELS_ALL:
    {
      ciCallbackEnumChannelsAllParams *callbackParams = data->callbackParams;
      chatEnumChannelsCallbackAll callback = data->callback;
      callback(chat, SUCCESS, NUM_CHANNELS, (const char **) CHANNELS, (const char **) TOPICS, NUM_USERS, param);
      break;
    }

    case CALLBACK_ENTER_CHANNEL:
    {
      ciCallbackEnterChannelParams *callbackParams = data->callbackParams;
      chatEnterChannelCallback callback = data->callback;

      // Call this before the callback so funcs called within the callback know.
      /
      ciJoinCallbackCalled(chat, CHANNEL);

      callback(chat, SUCCESS, RESULT, CHANNEL, param);
      break;
    }

    case CALLBACK_GET_CHANNEL_TOPIC:
    {
      ciCallbackGetChannelTopicParams *callbackParams = data->callbackParams;
      chatGetChannelTopicCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, TOPIC, param);
      break;
    }

    case CALLBACK_GET_CHANNEL_MODE:
    {
      ciCallbackGetChannelModeParams *callbackParams = data->callbackParams;
      chatGetChannelModeCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, MODE, param);
      break;
    }

    case CALLBACK_GET_CHANNEL_PASSWORD:
    {
      ciCallbackGetChannelPasswordParams *callbackParams = data->callbackParams;
      chatGetChannelPasswordCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, ENABLED, PASSWORD, param);
      break;
    }

    case CALLBACK_ENUM_USERS:
    {
      ciCallbackEnumUsersParams *callbackParams = data->callbackParams;
      chatEnumUsersCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, NUM_USERS, (const char **) USERS, MODES, param);
      break;
    }

    case CALLBACK_GET_USER_INFO:
    {
      ciCallbackGetUserInfoParams *callbackParams = data->callbackParams;
      chatGetUserInfoCallback callback = data->callback;
      callback(chat, SUCCESS, NICK, USER, NAME, ADDRESS, NUM_CHANNELS, (const char **) CHANNELS, param);
      break;
    }

    case CALLBACK_GET_BASIC_USER_INFO:
    {
      ciCallbackGetBasicUserInfoParams *callbackParams = data->callbackParams;
      chatGetBasicUserInfoCallback callback = data->callback;
      callback(chat, SUCCESS, NICK, USER, ADDRESS, param);
      break;
    }

    case CALLBACK_GET_CHANNEL_BASIC_USER_INFO:
    {
      ciCallbackGetChannelBasicUserInfoParams *callbackParams = data->callbackParams;
      chatGetChannelBasicUserInfoCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, NICK, USER, ADDRESS, param);
      break;
    }

    case CALLBACK_GET_USER_MODE:
    {
      ciCallbackGetUserModeParams *callbackParams = data->callbackParams;
      chatGetUserModeCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, USER, MODE, param);
      break;
    }

    case CALLBACK_ENUM_CHANNEL_BANS:
    {
      ciCallbackEnumChannelBansParams *callbackParams = data->callbackParams;
      chatEnumChannelBansCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, NUM_BANS, (const char **) BANS, param);
      break;
    }

    case CALLBACK_NICK_ERROR:
    {
      ciCallbackNickErrorParams *callbackParams = data->callbackParams;
      chatNickErrorCallback callback = data->callback;
      callback(chat, TYPE, NICK, param);
      break;
    }

    case CALLBACK_CHANGE_NICK:
    {
      ciCallbackChangeNickParams *callbackParams = data->callbackParams;
      chatChangeNickCallback callback = data->callback;
      callback(chat, SUCCESS, OLD_NICK, NEW_NICK, param);
      break;
    }

    case CALLBACK_NEW_USER_LIST:
    {
      ciCallbackNewUserListParams *callbackParams = data->callbackParams;
      chatNewUserList callback = data->callback;
      callback(chat, CHANNEL, NUM_USERS, (const char **) USERS, MODES, param);
      break;
    }

    case CALLBACK_BROADCAST_KEY_CHANGED:
    {
      ciCallbackBroadcastKeyChangedParams *callbackParams = data->callbackParams;
      chatBroadcastKeyChanged callback = data->callback;
      callback(chat, CHANNEL, USER, KEY, VALUE, param);
      break;
    }

    case CALLBACK_GET_GLOBAL_KEYS:
    {
      ciCallbackGetGlobalKeysParams *callbackParams = data->callbackParams;
      chatGetGlobalKeysCallback callback = data->callback;
      callback(chat, SUCCESS, USER, NUM, KEYS, VALUES, param);
      break;
    }

    case CALLBACK_GET_CHANNEL_KEYS:
    {
      ciCallbackGetChannelKeysParams *callbackParams = data->callbackParams;
      chatGetChannelKeysCallback callback = data->callback;
      callback(chat, SUCCESS, CHANNEL, USER, NUM, KEYS, VALUES, param);
      break;
    }

    default:
      // The type for this callback is messed up.
      ///////////////////////////////////////////
      assert(0);
  }

  // gsifree the data.
  /////////////////
  ciFreeCallbackData(data);
}

void ciCallCallbacks(CHAT chat, int ID)
{
  ciCallbackData *data;
  ciCallbackData dataCopy;
  int skip;
  CONNECTION;

  // Call the callbacks.
  //////////////////////
  for (skip = 0; ArrayLength(connection->callbackList) > skip;)
  {
    // Get the callback.
    ///////////////////
    data = (ciCallbackData *) ArrayNth(connection->callbackList, skip);
    ASSERT_DATA(data);

    // Does this depend on a channel we're not in?
    //////////////////////////////////////////////
    if ((data->channel != NULL) && !ciInChannel(chat, data->channel))
    {
      // gsifree the data.
      /////////////////
      ciFreeCallbackData(data);

      // Kill it.
      ///////////
      ArrayDeleteAt(connection->callbackList, skip);
    }
    else
    {
      // Check if this callback depends on the join callback having been called.
      // Also, if blocking, only call that callback.
      /
      if (((data->channel == NULL) || ciWasJoinCallbackCalled(chat, data->channel)) &&
          ((ID == 0) || (data->ID == ID)))
      {
        // Copy the data so we can gsifree it before calling the callback.
        ///////////////////////////////////////////////////////////////
        dataCopy = *data;

        // gsifree it.
        ///////////
        ArrayDeleteAt(connection->callbackList, skip);

        // Call the callback.
        /////////////////////
        ciCallCallback(chat, &dataCopy);

        // Was this the blocking callback?
        //////////////////////////////////
        if (ID != 0) return;
      }
      else
      {
        // Increment the skip, because it's still in the array.
        ///////////////////////////////////////////////////////
        skip++;
      }
    }
  }
}

static int ciGetCallbackIndexByID(CHAT chat, int ID)
{
  ciCallbackData *data;
  int i;
  int len;
  CONNECTION;

  // Get the array length.
  ////////////////////////
  len = ArrayLength(connection->callbackList);

  // Loop through the callbacks.
  //////////////////////////////
  for (i = 0; i < len; i++)
  {
    // Get the callback.
    ////////////////////
    data = (ciCallbackData *) ArrayNth(connection->callbackList, i);
    ASSERT_DATA(data);

    // Check for an ID match.
    /////////////////////////
    if (data->ID == ID) return i;
  }

  // Didn't find one.
  ///////////////////
  return -1;
}

CHATBool ciCheckCallbacksForID(CHAT chat, int ID)
{
  if (ciGetCallbackIndexByID(chat, ID) == -1) return CHATFalse;

  return CHATTrue;
}