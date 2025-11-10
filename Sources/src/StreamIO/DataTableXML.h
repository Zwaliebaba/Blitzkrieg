#ifndef __DATATABLEXML_H__
#define __DATATABLEXML_H__

#pragma once

#import "msxml.dll"

using namespace MSXML;

class CDataTableXML : public IDataTable
{
  OBJECT_MINIMAL_METHODS(CDataTableXML);
  //
  CPtr<IDataStream> pStream;// stream, this table was open with
  IXMLDOMDocumentPtr xmlDocument;// open document
  IXMLDOMNodePtr xmlRootNode;// root node
  //
  bool bModified;
  // get text node by name. 
  IXMLDOMNodePtr GetTextNode(const char *pszRow, const char *pszEntry)
  {
    IXMLDOMNodePtr xmlCurrNode = xmlRootNode->selectSingleNode(pszRow);
    if (xmlCurrNode == 0) return 0;
    IXMLDOMNodePtr xmlNode = xmlCurrNode->attributes->getNamedItem(pszEntry);
    if (xmlNode == 0) xmlNode = xmlCurrNode->selectSingleNode(pszEntry);
    return xmlNode;
  }

  //
  void SetModified() { bModified = true; }
  //
  template<class TYPE>
  void SetValue(const char *pszRow, const char *pszEntry, const TYPE &val)
  {
    if (IXMLDOMElementPtr pElement = xmlRootNode->selectSingleNode(pszRow)) pElement->setAttribute(pszEntry, val);
    else
    {
      IXMLDOMElementPtr pElement = xmlDocument->createElement(pszRow);
      xmlRootNode->appendChild(pElement);
      pElement->setAttribute(pszEntry, val);
    }
  }

  //
  IXMLDOMNodePtr GetNode(const std::string &szName);

  const std::string MakeName(const char *pszRow, const char *pszEntry)
  {
    std::string szName = std::string(pszRow) + "." + std::string(pszEntry);
    std::replace_if(szName.begin(), szName.end(), std::bind2nd(std::equal_to<char>(), '.'), '/');
    return szName;
  }

public:
  CDataTableXML();
  virtual ~CDataTableXML();
  //
  bool Open(IDataStream *pStream, const char *pszBaseNode);
  // get table row names. 
  int STDCALL GetRowNames(char *pszBuffer, int nBufferSize) override;
  // get the names of table columns in a given row. 
  int STDCALL GetEntryNames(const char *pszRow, char *pszBuffer, int nBufferSize) override;
  // section cleaning
  void STDCALL ClearRow(const char *pszRowName) override {}
  // complete element access
  // get
  int STDCALL GetInt(const char *pszRow, const char *pszEntry, int defval) override;
  double STDCALL GetDouble(const char *pszRow, const char *pszEntry, double defval) override;
  const char * STDCALL GetString(const char *pszRow, const char *pszEntry, const char *defval, char *pszBuffer, int nBufferSize) override;
  int STDCALL GetRawData(const char *pszRow, const char *pszEntry, void *pBuffer, int nBufferSize) override;
  // set
  void STDCALL SetInt(const char *pszRow, const char *pszEntry, int val) override;
  void STDCALL SetDouble(const char *pszRow, const char *pszEntry, double val) override;
  void STDCALL SetString(const char *pszRow, const char *pszEntry, const char *val) override;
  void STDCALL SetRawData(const char *pszRow, const char *pszEntry, const void *pBuffer, int nBufferSize) override;
};

#endif // __DATATABLEXML_H__