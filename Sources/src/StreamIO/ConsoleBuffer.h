#ifndef __CONSOLE_BUFFER_H__
#define __CONSOLE_BUFFER_H__

#pragma once

class CConsoleBuffer : public IConsoleBuffer
{
  OBJECT_NORMAL_METHODS(CConsoleBuffer);
  // DECLARE_SERIALIZE;
  //
  using CStringsList = std::list<std::pair<std::wstring, DWORD>>;
  using CStreamsMap = std::unordered_map<int, CStringsList>;
  using CDublicateMap = std::unordered_map<int, std::list<int>>;
  using CStreamNamesMap = std::unordered_map<int, std::string>;
  CStreamsMap streams;// all stream channels map
  CStreamNamesMap names;// some channel names
  CStreamsMap logs;// channel logs
  CDublicateMap dublicates;// duplicate channels info
  std::string szDumpFileName;// dump file name
  //
  std::wstring szTempString;// temp string to return values
  std::string szTempStringASCII;// ASCII temp string
  std::string szLogFileName;// log file name
  //
  void WriteLocal(int nStreamID, const wchar_t *pszString, DWORD color, bool bBackupLog);
  //
  virtual ~CConsoleBuffer();

public:
  // configure console buffer
  bool STDCALL Configure(const char *pszConfigure) override;
  // write string to console's stream
  void STDCALL Write(int nStreamID, const wchar_t *pszString, DWORD color = 0xffffffff, bool bBackupLog = false) override;
  // write string to console's stream. 
  void STDCALL WriteASCII(int nStreamID, const char *pszString, DWORD color = 0xffffffff, bool bBackupLog = false) override;
  // read string from console's stream
  const wchar_t * STDCALL Read(int nStreamID, DWORD *pColor = nullptr) override;
  // read string from console's stream. 
  const char * STDCALL ReadASCII(int nStreamID, DWORD *pColor = nullptr) override;
  // dump console's stream log to the previously configured output devices
  bool STDCALL DumpLog(int nStreamID) override;
};

#endif // __CONSOLE_BUFFER_H__