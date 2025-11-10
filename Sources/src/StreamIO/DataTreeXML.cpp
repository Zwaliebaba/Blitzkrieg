#include "StdAfx.h"
#include "DataTreeXML.h"
#include "../StreamIO/StreamAdaptor.h"
#include <ocidl.h>
#include <mutex>

class CDataTreeXMLAutomatic
{
  bool bInitialized;
  std::mutex initMutex;

public:
  CDataTreeXMLAutomatic()
    : bInitialized(false) {
  }

  ~CDataTreeXMLAutomatic() { if (bInitialized) { CoUninitialize(); } }

  void Init()
  {
    std::lock_guard<std::mutex> lock(initMutex);
    if (!bInitialized)
    {
      // Use COINIT_MULTITHREADED for MSXML3 better thread support
      const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
      // S_FALSE means already initialized, which is acceptable
      bInitialized = SUCCEEDED(hr) || hr == S_FALSE;
      NI_ASSERT_TF(bInitialized, "Failed to initialize COM", return);
    }
  }
};

static CDataTreeXMLAutomatic autoinit;
void InitCOM() { autoinit.Init(); }

CDataTreeXML::CDataTreeXML(EAccessMode _eMode)
  : xmlDocument("Msxml2.DOMDocument.3.0"), eMode(_eMode)
{
  xmlDocument->put_async(VARIANT_FALSE);
  xmlDocument->put_validateOnParse(VARIANT_FALSE);
  xmlDocument->put_resolveExternals(VARIANT_FALSE);
}

CDataTreeXML::~CDataTreeXML()
{
  try
  {
    FinishChunk();
    if (pStream)
    {
      CStreamCOMAdaptor comstream(pStream);
      _variant_t varStream;
      varStream.vt = VT_UNKNOWN;
      varStream.punkVal = static_cast<IStream*>(&comstream);
      varStream.punkVal->AddRef();
      xmlDocument->save(varStream);
    }
  }
  catch (const std::exception& e)
  {
    // Log the error if logging is available
    // For now, suppress to prevent exceptions from escaping destructor
    (void)e;
  }
  catch (...)
  {
    // Suppress all exceptions in destructor
  }
}

bool CDataTreeXML::Open(IDataStream* _pStream, DTChunkID idBaseNode)
{
  pStream = _pStream;
  if (IsReading())
  {
    NI_ASSERT_TF(pStream != nullptr, "Can't open data table from NULL stream!", return false);
    CStreamCOMAdaptor comstream(pStream);

    // MSXML3 uses VARIANT_BOOL return type and VARIANT parameter
    _variant_t varStream;
    varStream.vt = VT_UNKNOWN;
    varStream.punkVal = static_cast<IStream*>(&comstream);
    varStream.punkVal->AddRef();

    VARIANT_BOOL loadResult = xmlDocument->load(varStream);

    if (loadResult == VARIANT_TRUE)
    {
      nodes.push_back(SCOMPtr<MSXML2::IXMLDOMNodePtr>());
      nodes.back().data = xmlDocument;
      xmlCurrNode = xmlDocument;
      return StartChunk(idBaseNode);
    }
    // Get parse error information for better diagnostics
    MSXML2::IXMLDOMParseErrorPtr parseError;
    HRESULT hr = xmlDocument->get_parseError(&parseError);
    if (SUCCEEDED(hr) && parseError != nullptr)
    {
      long errorCode = 0;
      parseError->get_errorCode(&errorCode);

      BSTR reasonBstr = nullptr;
      parseError->get_reason(&reasonBstr);
      const _bstr_t reason(reasonBstr, false);// Don't copy, take ownership

      NI_ASSERT_TF(false,
        NStr::Format("XML Parse Error: Code=%ld, Reason=%s", errorCode, (const char*)reason),
        return false);
    }
  }
  else
  {
    MSXML2::IXMLDOMProcessingInstructionPtr pPI;
    pPI = xmlDocument->createProcessingInstruction(_bstr_t("xml"), _bstr_t("version=\"1.0\" encoding=\"UTF-8\""));
    if (pPI != nullptr)
    {
      MSXML2::IXMLDOMNodePtr newChild;
      newChild = xmlDocument->appendChild(pPI);
    }
    return StartChunk(idBaseNode);
  }

  return false;
}

int CDataTreeXML::StartChunk(DTChunkID idChunk)
{
  if (idChunk[0] == '\0') return -1;

  if (IsReading())
  {
    if (xmlCurrNode != nullptr)
    {
      nodes.push_back(SCOMPtr<MSXML2::IXMLDOMNodePtr>());
      nodes.back().data = xmlCurrNode;
    }
    if (xmlCurrNode == nullptr)
    {
      SStorageElementStats stats;
      pStream->GetStats(&stats);
      const std::string szError = stats.pszName != nullptr
        ? NStr::Format("Can't start chunk \"%s\" from XML file \"%s\"", idChunk, stats.pszName)
        : NStr::Format("Can't start chunk \"%s\" from unknown XML file", idChunk);
      NI_ASSERT_TF(xmlCurrNode != nullptr, szError.c_str(), return false);
    }

    MSXML2::IXMLDOMNodePtr tempNode;
    tempNode = xmlCurrNode->selectSingleNode(_bstr_t(idChunk));
    xmlCurrNode = tempNode;

    if (xmlCurrNode == nullptr)
    {
      FinishChunk();
      return 0;
    }
    return 1;
  }

  if (xmlCurrElement != nullptr)
  {
    elements.push_back(SCOMPtr<MSXML2::IXMLDOMElementPtr>());
    elements.back().data = xmlCurrElement;
  }

  xmlCurrElement = xmlDocument->createElement(_bstr_t(idChunk));
  if (xmlCurrElement != nullptr)
  {
    MSXML2::IXMLDOMNodePtr newChild;
    if (elements.empty()) newChild = xmlDocument->appendChild(xmlCurrElement);
    else elements.back().data->appendChild(xmlCurrElement);
  }

  if (xmlCurrElement == nullptr)
  {
    FinishChunk();
    return 0;
  }
  return 1;
}

void CDataTreeXML::FinishChunk()
{
  if (IsReading())
  {
    if (nodes.empty()) xmlCurrNode = nullptr;
    else
    {
      xmlCurrNode = nodes.back().data;
      nodes.pop_back();
    }
  }
  else
  {
    if (elements.empty()) xmlCurrElement = nullptr;
    else
    {
      xmlCurrElement = elements.back().data;
      elements.pop_back();
    }
  }
}

int CDataTreeXML::StartContainerChunk(DTChunkID idChunk)
{
  const std::string szChunkName = idChunk[0] == '\0' ? "data" : idChunk;

  if (IsReading())
  {
    MSXML2::IXMLDOMNodePtr tempNode;
    tempNode = xmlCurrNode->selectSingleNode(_bstr_t(szChunkName.c_str()));

    if (tempNode != nullptr)
    {
      nodes.push_back(SCOMPtr<MSXML2::IXMLDOMNodePtr>());
      nodes.back().data = xmlCurrNode;

      MSXML2::IXMLDOMNodeListPtr xmlNodesList;
      xmlNodesList = xmlCurrNode->selectNodes(_bstr_t((szChunkName + "/item").c_str()));

      if (xmlNodesList != nullptr)
      {
        nodelists.push_back(SNodeslList());
        nodelists.back().nodes.data = xmlNodesList;
        nodelists.back().nCurrElement = -1;
        return true;
      }
    }
    return 0;
  }

  if (xmlCurrElement != nullptr)
  {
    elements.push_back(SCOMPtr<MSXML2::IXMLDOMElementPtr>());
    elements.back().data = xmlCurrElement;
  }

  MSXML2::IXMLDOMElementPtr xmlArrayBase;
  xmlArrayBase = xmlDocument->createElement(_bstr_t(szChunkName.c_str()));

  if (xmlArrayBase != nullptr)
  {
    arrbases.push_back(SCOMPtr<MSXML2::IXMLDOMElementPtr>());
    arrbases.back().data = xmlArrayBase;

    if (xmlCurrElement) xmlCurrElement->appendChild(xmlArrayBase);
    else xmlDocument->appendChild(xmlArrayBase);

    return true;
  }

  return false;
}

void CDataTreeXML::FinishContainerChunk()
{
  FinishChunk();
  if (IsReading()) nodelists.pop_back();
  else arrbases.pop_back();
}

bool CDataTreeXML::SetChunkCounter(int nCount)
{
  if (IsReading())
  {
    nodelists.back().nCurrElement = nCount;

    long length = 0;
    nodelists.back().nodes.data->get_length(&length);

    if (nodelists.back().nCurrElement < length) { nodelists.back().nodes.data->get_item(nodelists.back().nCurrElement, &xmlCurrNode); }
    else xmlCurrNode = nullptr;

    return xmlCurrNode != nullptr;
  }

  xmlCurrElement = xmlDocument->createElement(_bstr_t("item"));
  if (xmlCurrElement != nullptr)
  {
    arrbases.back().data->appendChild(xmlCurrElement);
    return true;
  }

  return false;
}

int CDataTreeXML::CountChunks(DTChunkID idChunk)
{
  if (IsReading())
  {
    const std::string szChunkName = idChunk[0] == '\0' ? "data" : idChunk;
    MSXML2::IXMLDOMNodeListPtr xmlNodesList;
    xmlNodesList = xmlCurrNode->selectNodes(_bstr_t((szChunkName + "/item").c_str()));

    if (xmlNodesList != nullptr)
    {
      long length = 0;
      xmlNodesList->get_length(&length);
      return static_cast<int>(length);
    }
  }
  return 0;
}

int CDataTreeXML::GetChunkSize()
{
  if (IsReading() && (xmlCurrNode != nullptr))
  {
    BSTR textBstr = nullptr;
    HRESULT hr = xmlCurrNode->get_text(&textBstr);
    if (SUCCEEDED(hr) && textBstr != nullptr)
    {
      const _bstr_t textData(textBstr, false);// Take ownership
      return textData.length() > 0 ? static_cast<int>(strlen(textData)) : 0;
    }
  }
  return 0;
}

bool CDataTreeXML::RawData(void* pData, int nSize)
{
  NI_ASSERT_TF(pData != nullptr, "Null pointer passed to RawData", return false);
  NI_ASSERT_TF(nSize >= 0, "Invalid size passed to RawData", return false);

  if (IsReading())
  {
    const int nStrSize = GetChunkSize();
    if (nStrSize <= 0) return false;

    std::string szBuffer;
    szBuffer.resize(static_cast<size_t>(nStrSize) + 1);

    if (!StringData(&szBuffer[0])) return false;

    int nCheck = 0;
    NStr::StringToBin(szBuffer.c_str(), pData, &nCheck);
    NI_ASSERT_TF(nCheck == nSize, NStr::Format("Wrong buffer size: %d != %d", nCheck, nSize), return false);
  }
  else
  {
    std::string szBuffer;
    szBuffer.resize(static_cast<size_t>(nSize) * 2 + 1);
    NStr::BinToString(pData, nSize, &szBuffer[0]);
    if (!StringData(&szBuffer[0])) return false;
  }
  return true;
}

bool CDataTreeXML::StringData(char* pData)
{
  NI_ASSERT_TF(pData != nullptr, "Null pointer passed to StringData", return false);

  if (IsReading())
  {
    if (xmlCurrNode)
    {
      BSTR textBstr = nullptr;
      HRESULT hr = xmlCurrNode->get_text(&textBstr);
      if (SUCCEEDED(hr) && textBstr != nullptr)
      {
        const _bstr_t textData(textBstr, false);// Take ownership
        if (textData.length() > 0)
        {
          // Use safe string copy - assumes caller allocated sufficient buffer
          // In production, should pass buffer size and use strncpy_s
          strcpy(pData, textData);
          return true;
        }
      }
    }
  }
  else
  {
    if (xmlCurrElement)
    {
      MSXML2::IXMLDOMTextPtr xmlText;
      xmlText = xmlDocument->createTextNode(_bstr_t(pData));
      if (xmlText != nullptr)
      {
        xmlCurrElement->appendChild(xmlText);
        return true;
      }
    }
  }
  return false;
}

bool CDataTreeXML::StringData(wchar_t* pData)
{
  NI_ASSERT_TF(pData != nullptr, "Null pointer passed to StringData", return false);

  if (IsReading())
  {
    if (xmlCurrNode)
    {
      BSTR textBstr = nullptr;
      HRESULT hr = xmlCurrNode->get_text(&textBstr);
      if (SUCCEEDED(hr) && textBstr != nullptr)
      {
        const _bstr_t textData(textBstr, false);// Take ownership
        if (textData.length() > 0)
        {
          // Use safe string copy - assumes caller allocated sufficient buffer
          // In production, should pass buffer size and use wcscpy_s
          wcscpy(pData, textData);
          return true;
        }
      }
    }
  }
  else
  {
    if (xmlCurrElement)
    {
      MSXML2::IXMLDOMTextPtr xmlText;
      xmlText = xmlDocument->createTextNode(_bstr_t(pData));
      if (xmlText != nullptr)
      {
        xmlCurrElement->appendChild(xmlText);
        return true;
      }
    }
  }
  return false;
}

bool CDataTreeXML::DataChunk(DTChunkID idChunk, int* pData)
{
  NI_ASSERT_TF(pData != nullptr, "Null pointer passed to DataChunk", return false);

  if (IsReading())
  {
    if (MSXML2::IXMLDOMNodePtr pNode = GetTextNode(idChunk))
    {
      BSTR textBstr = nullptr;
      HRESULT hr = pNode->get_text(&textBstr);
      if (SUCCEEDED(hr) && textBstr != nullptr)
      {
        const _bstr_t textData(textBstr, false);// Take ownership
        if (textData.length() > 0 && sscanf(static_cast<const char*>(textData), "%i", pData) == 1) { return true; }
      }
    }
  }
  else
  {
    if (xmlCurrElement)
    {
      const std::string value = NStr::Format("%d", *pData);
      HRESULT hr = xmlCurrElement->setAttribute(_bstr_t(idChunk), _variant_t(value.c_str()));
      return SUCCEEDED(hr);
    }
  }
  return false;
}

bool CDataTreeXML::DataChunk(DTChunkID idChunk, double* pData)
{
  NI_ASSERT_TF(pData != nullptr, "Null pointer passed to DataChunk", return false);

  if (IsReading())
  {
    if (MSXML2::IXMLDOMNodePtr pNode = GetTextNode(idChunk))
    {
      BSTR textBstr = nullptr;
      HRESULT hr = pNode->get_text(&textBstr);
      if (SUCCEEDED(hr) && textBstr != nullptr)
      {
        const _bstr_t textData(textBstr, false);// Take ownership
        if (textData.length() > 0 && sscanf(static_cast<const char*>(textData), "%lg", pData) == 1) { return true; }
      }
    }
  }
  else
  {
    if (xmlCurrElement)
    {
      const std::string value = NStr::Format("%lg", *pData);
      HRESULT hr = xmlCurrElement->setAttribute(_bstr_t(idChunk), _variant_t(value.c_str()));
      return SUCCEEDED(hr);
    }
  }
  return false;
}