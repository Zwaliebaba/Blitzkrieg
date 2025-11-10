#include "StdAfx.h"
#include "DataTableXML.h"
#include "DataTreeXML.h"
#include "../StreamIO/StreamAdaptor.h"

CDataTableXML::CDataTableXML()
  : xmlDocument("Msxml2.DOMDocument.3.0"), bModified(false)
{
  xmlDocument->async = VARIANT_FALSE;
}

CDataTableXML::~CDataTableXML()
{
  try
  {
    if (bModified && pStream)
    {
      CStreamCOMAdaptor comstream(pStream);
      _variant_t varStream;
      varStream.vt = VT_UNKNOWN;
      varStream.punkVal = static_cast<IStream*>(&comstream);
      varStream.punkVal->AddRef();
      xmlDocument->save(varStream);
    }
  }
  catch (...) {}
}

bool CDataTableXML::Open(IDataStream* _pStream, const char* pszBaseNode)
{
  try
  {
    pStream = _pStream;
    CStreamCOMAdaptor comstream(pStream);
    int nPos = pStream->GetPos();

    _variant_t varStream;
    varStream.vt = VT_UNKNOWN;
    varStream.punkVal = static_cast<IStream*>(&comstream);
    varStream.punkVal->AddRef();

    VARIANT_BOOL loadResult = xmlDocument->load(varStream);

    if (loadResult == VARIANT_TRUE)
    {
      xmlRootNode = xmlDocument->selectSingleNode(_bstr_t(pszBaseNode));
      pStream->Seek(nPos, STREAM_SEEK_SET);
    }
    else
    {
      MSXML2::IXMLDOMProcessingInstructionPtr pPI =
        xmlDocument->createProcessingInstruction(_bstr_t("xml"), _bstr_t("version=\"1.0\""));
      if (pPI != nullptr)
      {
        xmlDocument->appendChild(pPI);
      }

      MSXML2::IXMLDOMElementPtr pElement = xmlDocument->createElement(_bstr_t(pszBaseNode));
      if (pElement != nullptr)
      {
        xmlDocument->appendChild(pElement);
      }
    }
    return true;
  }
  catch (...) {}
  return false;
}

// ************************************************************************************************************************ //
// **
// ** 
// **
// **
// **
// ************************************************************************************************************************ //

MSXML2::IXMLDOMNodePtr CDataTableXML::GetNode(const std::string& szName)
{
  const int nPos = static_cast<int>(szName.rfind('/'));
  const std::string szRestName = szName.substr(nPos + 1);

  MSXML2::IXMLDOMNodePtr xmlCurrNode = xmlRootNode->selectSingleNode(_bstr_t(szName.substr(0, nPos).c_str()));

  if (xmlCurrNode != nullptr)
  {
    MSXML2::IXMLDOMNamedNodeMapPtr attributes = xmlCurrNode->attributes;
    if (attributes != nullptr)
    {
      MSXML2::IXMLDOMNodePtr xmlNode = attributes->getNamedItem(_bstr_t(szRestName.c_str()));
      if (xmlNode != nullptr)
        return xmlNode;
    }

    MSXML2::IXMLDOMNodePtr xmlNode = xmlCurrNode->selectSingleNode(_bstr_t(szRestName.c_str()));
    if (xmlNode != nullptr)
      return xmlNode;
  }
  return nullptr;
}

inline int AddToBuffer(const std::string& szName, char*& pszBuffer, const int nBufferSize, const int nTotalSize)
{
  NI_ASSERT_TF(nTotalSize + szName.size() + 2 < static_cast<size_t>(nBufferSize), "Buffer too small to add name", return 0);
  strcpy(pszBuffer, szName.c_str());
  pszBuffer += szName.size();
  *pszBuffer++ = '\0';
  return static_cast<int>(szName.size()) + 1;
}

int CDataTableXML::GetRowNames(char* pszBuffer, int nBufferSize)
{
  try
  {
    int nTotalSize = 0;
    MSXML2::IXMLDOMNodeListPtr pNodes = xmlRootNode->childNodes;

    if (pNodes != nullptr)
    {
      long length = pNodes->length;

      for (long i = 0; i < length; ++i)
      {
        MSXML2::IXMLDOMNodePtr node = pNodes->item[i];
        if (node != nullptr)
        {
          _bstr_t nodeName = node->nodeName;
          std::string szName = static_cast<const char*>(nodeName);
          nTotalSize += AddToBuffer(szName, pszBuffer, nBufferSize, nTotalSize);
        }
      }
    }
    *pszBuffer++ = '\0';
    //
    return nTotalSize + 1;
  }
  catch (...) {}
  return 0;
}

int CDataTableXML::GetEntryNames(const char* pszRow, char* pszBuffer, int nBufferSize)
{
  try
  {
    int nTotalSize = 0;
    MSXML2::IXMLDOMNodePtr pNode = xmlRootNode->selectSingleNode(_bstr_t(pszRow));

    if (pNode != nullptr)
    {
      // attributes
      MSXML2::IXMLDOMNamedNodeMapPtr attributes = pNode->attributes;
      if (attributes != nullptr)
      {
        long attrLength = attributes->length;

        for (long i = 0; i < attrLength; ++i)
        {
          MSXML2::IXMLDOMNodePtr attrNode = attributes->item[i];
          if (attrNode != nullptr)
          {
            _bstr_t nodeName = attrNode->nodeName;
            const std::string szName = static_cast<const char*>(nodeName);
            nTotalSize += AddToBuffer(szName, pszBuffer, nBufferSize, nTotalSize);
          }
        }
      }

      // named nodes
      MSXML2::IXMLDOMNodeListPtr pNodes = pNode->childNodes;
      if (pNodes != nullptr)
      {
        long length = pNodes->length;

        for (long i = 0; i < length; ++i)
        {
          MSXML2::IXMLDOMNodePtr childNode = pNodes->item[i];
          if (childNode != nullptr)
          {
            _bstr_t nodeName = childNode->nodeName;
            std::string szName = static_cast<const char*>(nodeName);

            if ((szName == "#comment") || (szName == "#text")) continue;

            std::string szRowName = std::string(pszRow) + "/" + szName;
            std::replace_if(szRowName.begin(), szRowName.end(), [](char c) { return c == '.'; }, '/');
            //
            char buffer[65536];
            const int nSize = GetEntryNames(szRowName.c_str(), buffer, 65536);
            if (nSize > 1)
            {
              const char* pos = buffer;
              while ((*pos != 0) && (pos - buffer <= nSize))
              {
                std::string szNewName = szName + "/" + pos;
                std::replace_if(szNewName.begin(), szNewName.end(), [](char c) { return c == '/'; }, '.');
                nTotalSize += AddToBuffer(szNewName, pszBuffer, nBufferSize, nTotalSize);
                pos = std::find(pos, static_cast<const char*>(buffer) + nSize, '\0') + 1;
              }
            }
            else { nTotalSize += AddToBuffer(szName, pszBuffer, nBufferSize, nTotalSize); }
          }
        }
      }
    }
    *pszBuffer++ = '\0';
    //
    return nTotalSize + 1;
  }
  catch (...) {}
  return 0;
}

// get
int CDataTableXML::GetInt(const char* pszRow, const char* pszEntry, int defval)
{
  const std::string szName = MakeName(pszRow, pszEntry);
  if (MSXML2::IXMLDOMNodePtr pNode = GetNode(szName))
  {
    _bstr_t text = pNode->text;
    return atoi(static_cast<const char*>(text));
  }
  return defval;
}

double CDataTableXML::GetDouble(const char* pszRow, const char* pszEntry, double defval)
{
  const std::string szName = MakeName(pszRow, pszEntry);
  if (MSXML2::IXMLDOMNodePtr pNode = GetNode(szName))
  {
    _bstr_t text = pNode->text;
    return atof(static_cast<const char*>(text));
  }
  return defval;
}

const char* CDataTableXML::GetString(const char* pszRow, const char* pszEntry, const char* defval, char* pszBuffer, int nBufferSize)
{
  const std::string szName = MakeName(pszRow, pszEntry);
  if (MSXML2::IXMLDOMNodePtr pNode = GetNode(szName))
  {
    _bstr_t text = pNode->text;
    std::string szString = static_cast<const char*>(text);
    NI_ASSERT_TF(nBufferSize >= static_cast<int>(szString.size()), "Buffer too small to fill all string", return nullptr);
    strcpy(pszBuffer, szString.c_str());
    return pszBuffer;
  }
  strcpy(pszBuffer, defval);
  return pszBuffer;
}

int CDataTableXML::GetRawData(const char* pszRow, const char* pszEntry, void* pBuffer, int nBufferSize)
{
  const std::string szName = MakeName(pszRow, pszEntry);
  if (MSXML2::IXMLDOMNodePtr pNode = GetNode(szName))
  {
    _bstr_t text = pNode->text;
    std::string szBuffer = static_cast<const char*>(text);
    NI_ASSERT_TF(nBufferSize >= static_cast<int>(szBuffer.size()), "Buffer too small to fill all string", return 0);
    int nCheck = static_cast<int>(szBuffer.size()) / 2;
    NI_ASSERT_TF(nCheck >= nBufferSize, NStr::Format("Wrong buffer size: %d >= %d", nCheck, nBufferSize), return 0);
    NStr::StringToBin(szBuffer.c_str(), pBuffer, &nCheck);
    return nCheck;
  }
  return 0;
}

// set
void CDataTableXML::SetInt(const char* pszRow, const char* pszEntry, int val)
{
  NI_ASSERT_T(false, "Still not implemented");
  SetValue(pszRow, pszEntry, static_cast<long>(val));
  SetModified();
}

void CDataTableXML::SetDouble(const char* pszRow, const char* pszEntry, double val)
{
  NI_ASSERT_T(false, "Still not implemented");
  SetValue(pszRow, pszEntry, val);
  SetModified();
}

void CDataTableXML::SetString(const char* pszRow, const char* pszEntry, const char* val)
{
  NI_ASSERT_T(false, "Still not implemented");

  MSXML2::IXMLDOMNodePtr pNode = xmlRootNode->selectSingleNode(_bstr_t(pszRow));

  if (pNode != nullptr)
  {
    MSXML2::IXMLDOMTextPtr xmlText = xmlDocument->createTextNode(_bstr_t(val));
    if (xmlText != nullptr)
    {
      pNode->appendChild(xmlText);
    }
  }
  else
  {
    MSXML2::IXMLDOMElementPtr pElement = xmlDocument->createElement(_bstr_t(pszRow));
    if (pElement != nullptr)
    {
      xmlRootNode->appendChild(pElement);

      MSXML2::IXMLDOMTextPtr xmlText = xmlDocument->createTextNode(_bstr_t(val));
      if (xmlText != nullptr)
      {
        pElement->appendChild(xmlText);
      }
    }
  }
  SetModified();
}

void CDataTableXML::SetRawData(const char* pszRow, const char* pszEntry, const void* pBuffer, int nBufferSize)
{
  NI_ASSERT_T(false, "Still not implemented");
  std::string szString;
  szString.resize(static_cast<size_t>(nBufferSize) * 2);
  NStr::BinToString(pBuffer, nBufferSize, &szString[0]);
  SetString(pszRow, pszEntry, szString.c_str());
  SetModified();
}