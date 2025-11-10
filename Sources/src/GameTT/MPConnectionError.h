#ifndef __MPCONNECTIONERROR_H__
#define __MPCONNECTIONERROR_H__

#pragma once

class CMPConnectionError
{
public:
  static bool DisplayError(enum EMultiplayerToUICommands eErrorID);
};

#endif // __MPCONNECTIONERROR_H__