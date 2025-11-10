#ifndef __DATATREEXML_H__
#define __DATATREEXML_H__

#pragma once

#import "msxml.dll"

using namespace MSXML;

// these two structures are specially created to allow dumping com_ptr objects into STL containers
// humpback, of course, but that’s the nature com_ptr - a complete mess
template<class TYPE>
struct SCOMPtr
{
  TYPE data;
};

struct SNodeslList
{
  SCOMPtr<IXMLDOMNodeListPtr> nodes;
  int nCurrElement;
  //
  SNodeslList() : nCurrElement(-1) {}
};

class CDataTreeXML : public IDataTree
{
  OBJECT_MINIMAL_METHODS(CDataTreeXML);
  //
  CPtr<IDataStream> pStream;// stream, this table was open with
  IXMLDOMDocumentPtr xmlDocument;// open document
  //
  std::list<SCOMPtr<IXMLDOMNodePtr>> nodes;// stack of nodes according to the deepening hierarchy
  std::list<SNodeslList> nodelists;// stack of lists of nodes according to the deepening hierarchy
  IXMLDOMNodePtr xmlCurrNode;// current node
  //
  std::list<SCOMPtr<IXMLDOMElementPtr>> elements;// stack of elements according to deepening hierarchy
  std::list<SCOMPtr<IXMLDOMElementPtr>> arrbases;// stack of elements of arrays according to the deepening hierarchy
  IXMLDOMElementPtr xmlCurrElement;// current element in block structure when written
  //
  EAccessMode eMode;
  //
  // get an attribute by name from the current node.
  IXMLDOMNodePtr GetAttribute(DTChunkID idChunk)
  {
    NI_ASSERT_TF(xmlCurrNode != 0, "can't get attribute - no current node set", return 0);
    return xmlCurrNode->attributes->getNamedItem(idChunk);
  }

  // get text node by name. 
  IXMLDOMNodePtr GetTextNode(DTChunkID idChunk)
  {
    NI_ASSERT_TF(xmlCurrNode != 0, "can't get node - no current node set", return 0);
    IXMLDOMNodePtr xmlNode = xmlCurrNode->attributes->getNamedItem(idChunk);
    if (xmlNode == 0) xmlNode = xmlCurrNode->selectSingleNode(idChunk);
    return xmlNode;
  }

public:
  CDataTreeXML(EAccessMode eMode);
  virtual ~CDataTreeXML();
  //
  bool Open(IDataStream *pStream, DTChunkID idBaseNode);
  // is opened in the READ mode?
  bool STDCALL IsReading() const override { return eMode == READ; }
  // start new complex chunk
  int STDCALL StartChunk(DTChunkID idChunk) override;
  // finish complex chunk
  void STDCALL FinishChunk() override;
  // simply data chunk: text, integer, fp
  int STDCALL GetChunkSize() override;
  bool STDCALL RawData(void *pData, int nSize) override;
  bool STDCALL StringData(char *pData) override;
  bool STDCALL StringData(WORD *pData) override;
  bool STDCALL DataChunk(DTChunkID idChunk, int *pData) override;
  bool STDCALL DataChunk(DTChunkID idChunk, double *pData) override;
  // array data serialization (special case)
  int STDCALL CountChunks(DTChunkID idChunk) override;
  bool STDCALL SetChunkCounter(int nCount) override;
  int STDCALL StartContainerChunk(DTChunkID idChunk) override;
  void STDCALL FinishContainerChunk() override;
};

void InitCOM();

#endif // __DATATREEXML_H__