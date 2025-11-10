#pragma once

#import <msxml3.dll>
using namespace MSXML2;

// These two structures are specially created to allow referencing com_ptr objects in STL containers
// Rough, admittedly, but this com_ptr wrapper is sufficient
template<class TYPE>
struct SCOMPtr
{
  TYPE data;
};

struct SNodeslList
{
  SCOMPtr<MSXML2::IXMLDOMNodeListPtr> nodes;
  int nCurrElement;

  SNodeslList() : nCurrElement(-1) {}
};

class CDataTreeXML : public IDataTree
{
  OBJECT_MINIMAL_METHODS(CDataTreeXML);

  CPtr<IDataStream> pStream;// stream this table was opened with
  MSXML2::IXMLDOMDocumentPtr xmlDocument;// opened document

  std::list<SCOMPtr<MSXML2::IXMLDOMNodePtr>> nodes;// stack of nodes by nesting hierarchy
  std::list<SNodeslList> nodelists;// stack of node lists by nesting hierarchy
  MSXML2::IXMLDOMNodePtr xmlCurrNode;// current node

  std::list<SCOMPtr<MSXML2::IXMLDOMElementPtr>> elements;// stack of elements by nesting hierarchy
  std::list<SCOMPtr<MSXML2::IXMLDOMElementPtr>> arrbases;// stack of elements that are array bases by nesting hierarchy
  MSXML2::IXMLDOMElementPtr xmlCurrElement;// current element in the block structure when writing

  EAccessMode eMode;

  // get attribute node by name from the current node.
  MSXML2::IXMLDOMNodePtr GetAttribute(DTChunkID idChunk)
  {
    NI_ASSERT_TF(xmlCurrNode != nullptr, "can't get attribute - no current node set", return nullptr);

    MSXML2::IXMLDOMNamedNodeMapPtr attributes;
    HRESULT hr = xmlCurrNode->get_attributes(&attributes);
    if (SUCCEEDED(hr) && attributes != nullptr)
    {
      MSXML2::IXMLDOMNodePtr attrNode;
      attrNode = attributes->getNamedItem(_bstr_t(idChunk));
      if (attrNode != nullptr) return attrNode;
    }
    return nullptr;
  }

  // get a text node by name. This may be an attribute of the current node or a single child node of the current node.
  MSXML2::IXMLDOMNodePtr GetTextNode(DTChunkID idChunk)
  {
    NI_ASSERT_TF(xmlCurrNode != nullptr, "can't get node - no current node set", return nullptr);

    MSXML2::IXMLDOMNodePtr xmlNode;

    // Try to get as attribute first
    MSXML2::IXMLDOMNamedNodeMapPtr attributes;
    HRESULT hr = xmlCurrNode->get_attributes(&attributes);
    if (SUCCEEDED(hr) && attributes != nullptr)
    {
      xmlNode = attributes->getNamedItem(_bstr_t(idChunk));
      if (xmlNode != nullptr) return xmlNode;
    }

    // If not found as attribute, try as child node
    xmlNode = xmlCurrNode->selectSingleNode(_bstr_t(idChunk));
    return xmlNode;
  }

public:
  CDataTreeXML(EAccessMode eMode);
  virtual ~CDataTreeXML();

  bool Open(IDataStream* pStream, DTChunkID idBaseNode);
  // is opened in the READ mode?
  bool STDCALL IsReading() const override { return eMode == READ; }
  // start new complex chunk
  int STDCALL StartChunk(DTChunkID idChunk) override;
  // finish complex chunk
  void STDCALL FinishChunk() override;
  // simply data chunk: text, integer, fp
  int STDCALL GetChunkSize() override;
  bool STDCALL RawData(void* pData, int nSize) override;
  bool STDCALL StringData(char* pData) override;
  bool STDCALL StringData(wchar_t* pData) override;
  bool STDCALL DataChunk(DTChunkID idChunk, int* pData) override;
  bool STDCALL DataChunk(DTChunkID idChunk, double* pData) override;
  // array data serialization (special case)
  int STDCALL CountChunks(DTChunkID idChunk) override;
  bool STDCALL SetChunkCounter(int nCount) override;
  int STDCALL StartContainerChunk(DTChunkID idChunk) override;
  void STDCALL FinishContainerChunk() override;
};

void InitCOM();