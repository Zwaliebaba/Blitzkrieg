#ifndef __DATATABLEXML_H__
#define __DATATABLEXML_H__

#pragma once

#import <msxml3.dll>
using namespace MSXML2;

class CDataTableXML : public IDataTable
{
  OBJECT_MINIMAL_METHODS(CDataTableXML);
  //
  CPtr<IDataStream> pStream;// stream, this table was open with
  MSXML2::IXMLDOMDocumentPtr xmlDocument;// opened document
  MSXML2::IXMLDOMNodePtr xmlRootNode;// root node
  //
  bool bModified;
  // get text node by name. This is either an attribute of the current node, or a single node from the current one.
  MSXML2::IXMLDOMNodePtr GetTextNode(const char* pszRow, const char* pszEntry)
  {
    MSXML2::IXMLDOMNodePtr xmlCurrNode = xmlRootNode->selectSingleNode(_bstr_t(pszRow));
    if (xmlCurrNode == nullptr) return nullptr;

    MSXML2::IXMLDOMNamedNodeMapPtr attributes = xmlCurrNode->attributes;
    if (attributes != nullptr)
    {
      MSXML2::IXMLDOMNodePtr xmlNode = attributes->getNamedItem(_bstr_t(pszEntry));
      if (xmlNode != nullptr)
        return xmlNode;
    }

    MSXML2::IXMLDOMNodePtr xmlNode = xmlCurrNode->selectSingleNode(_bstr_t(pszEntry));
    if (xmlNode != nullptr)
      return xmlNode;

    return nullptr;
  }

  //
  void SetModified() { bModified = true; }
  //
  template<class TYPE>
  void SetValue(const char* pszRow, const char* pszEntry, const TYPE& val)
  {
    MSXML2::IXMLDOMElementPtr pElement = xmlRootNode->selectSingleNode(_bstr_t(pszRow));

    if (pElement != nullptr)
    {
      pElement->setAttribute(_bstr_t(pszEntry), _variant_t(val));
    }
    else
    {
      MSXML2::IXMLDOMElementPtr pNewElement = xmlDocument->createElement(_bstr_t(pszRow));
      if (pNewElement != nullptr)
      {
        xmlRootNode->appendChild(pNewElement);
        pNewElement->setAttribute(_bstr_t(pszEntry), _variant_t(val));
      }
    }
  }

  //
  MSXML2::IXMLDOMNodePtr GetNode(const std::string& szName);

  const std::string MakeName(const char* pszRow, const char* pszEntry)
  {
    std::string szName = std::string(pszRow) + "." + std::string(pszEntry);
    std::replace_if(szName.begin(), szName.end(), [](char c) { return c == '.'; }, '/');
    return szName;
  }

public:
  CDataTableXML();
  virtual ~CDataTableXML();
  //
  bool Open(IDataStream* pStream, const char* pszBaseNode);
  // get table row names. each name ends with '\0', the entire string ends with '\0\0'
  int STDCALL GetRowNames(char* pszBuffer, int nBufferSize) override;
  // get table column names in the given row. each name ends with '\0', the entire string ends with '\0\0'
  int STDCALL GetEntryNames(const char* pszRow, char* pszBuffer, int nBufferSize) override;
  // clear section
  void STDCALL ClearRow(const char* pszRowName) override {}
  // complete element access
  // get
  int STDCALL GetInt(const char* pszRow, const char* pszEntry, int defval) override;
  double STDCALL GetDouble(const char* pszRow, const char* pszEntry, double defval) override;
  const char* STDCALL GetString(const char* pszRow, const char* pszEntry, const char* defval, char* pszBuffer, int nBufferSize) override;
  int STDCALL GetRawData(const char* pszRow, const char* pszEntry, void* pBuffer, int nBufferSize) override;
  // set
  void STDCALL SetInt(const char* pszRow, const char* pszEntry, int val) override;
  void STDCALL SetDouble(const char* pszRow, const char* pszEntry, double val) override;
  void STDCALL SetString(const char* pszRow, const char* pszEntry, const char* val) override;
  void STDCALL SetRawData(const char* pszRow, const char* pszEntry, const void* pBuffer, int nBufferSize) override;
};

#endif // __DATATABLEXML_H__