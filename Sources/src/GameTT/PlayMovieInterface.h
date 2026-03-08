#ifndef __PLAYMOVIEINTERFACE_H__
#define __PLAYMOVIEINTERFACE_H__

#pragma once

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "iMission.h"

class CPlayMovieInterface : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CPlayMovieInterface);
  DECLARE_SERIALIZE;

  //
  struct SMovie
  {
    std::string szFileName;// file name (with respect to 'data' subdir)
    DWORD dwFrameDelay;// interframe delay for this movie (for 'slideshow' mode)
    bool bCanInterupt;// can we interact this movie with mouse/SPACE/ENTER/ESC buttons
    bool bCanSkipFrame;// can we skip frame (for 'slideshow' mode)
    //
    int operator&(IDataTree &ss)
    {
      CTreeAccessor saver = &ss;
      saver.Add("FileName", &szFileName);
      saver.Add("FrameDelay", &dwFrameDelay);
      saver.Add("CanInterupt", &bCanInterupt);
      saver.Add("CanSkipFrame", &bCanSkipFrame);
      return 0;
    }

    int operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &szFileName);
      saver.Add(2, &dwFrameDelay);
      saver.Add(3, &bCanInterupt);
      saver.Add(4, &bCanSkipFrame);
      return 0;
    }
  };

  // input
  NInput::CCommandRegistrator movieMsgs;
  //
  CPtr<IVideoPlayer> pPlayer;// video player with current bink video
  std::vector<SMovie> movies;// all movies to play
  int nCurrMovie;// current movie to play
  // next interface
  int nNextInterfaceCommandTypeID;
  std::string szNextInterfaceCommandConfig;
  //
  bool PlayMovie();
  void StartNextInterface();
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  bool OpenCurtains() override { return false; }

public:
  CPlayMovieInterface();
  ~CPlayMovieInterface() override;
  //
  void LoadMovieSequence(const std::string &szFileName);
  void SetNextInterface(int nTypeID, const std::string &szConfig);
  //
  bool STDCALL Init() override;
  void STDCALL Done() override;
  void STDCALL Step(bool bAppActive) override;
  void STDCALL OnGetFocus(bool bFocus) override;
};

class CICPlayMovie : public CInterfaceCommandBase<CPlayMovieInterface, MISSION_INTERFACE_VIDEO>
{
  OBJECT_NORMAL_METHODS(CICPlayMovie);
  DECLARE_SERIALIZE;
  //
  std::string szSequenceName;
  int nNextICTypeID;
  std::string szNextICConfig;
  //
  void PreCreate(IMainLoop *pML) override { pML->PopInterface(); }
  void PostCreate(IMainLoop *pML, CPlayMovieInterface *pInterface) override;
  //
  CICPlayMovie() {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif // __PLAYMOVIEINTERFACE_H__