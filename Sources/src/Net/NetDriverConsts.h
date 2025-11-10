#ifndef __NET_DRIVER_CONSTS_H__
#define __NET_DRIVER_CONSTS_H__

#pragma once

struct SNetDriverConsts
{
  // time after which client drop
  static int F_TIMEOUT;
  // time after which the server is removed from serverslist
  static float FP_SERVER_LIST_TIMEOUT;

  static void Load();
};

#endif // __NET_DRIVER_CONSTS_H__