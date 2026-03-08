#ifndef __FMTSPRITE_H__
#define __FMTSPRITE_H__

#pragma once


// DESCRIPTION: this array is a special data structure used
// in the SImageEdge class for storing image borders. 
// sequences T of variable length (length is of type TCounter). 
// elements of the array are followed. 
// CVarArray2D[ lineIndex ][ inLineIndex ]
// The array is created as follows. 
// the method is called: SetSizes( int nSize, int nLineCount ) or
// constructor: CVarArray2D( int nSize, int nLineCount )
// after, for each line (in order: 0..nLineCount), the method is called:
// SetLineLength( int lineIndex, TCounter nLineLength )
// after this the array is ready for use.
// NOTE: the sum of nLineLength fields in SetLineLength calls must be exactly
// match the length of the nSize array specified in the constructor or in the SetSizes method
// TIP: do not use this array if you do not know the specifics of its operation and creation
// T - array element type
// TCounter - type of counter by line of elements
template<class T, class TCounter = BYTE>
class CVarArray2D
{
  using PT = T *;

  int nSize;// total elements count
  int nLineCount;// data line array size
  T *pData;// main data pointer (nSize)
  T **pLineData;// data lines pointers ( nLineCount )
  TCounter *pLineDataSize;// data line sizes pointer ( nLineCount )

  void IndexLineData()
  {
    int index = 0;
    int count = 0;
    for (index = 0; index < nLineCount; ++index)
    {
      pLineData[index] = pData + count;
      count += pLineDataSize[index];
    }
  }

  void Copy(const CVarArray2D &varArray2D)
  {
    nSize = varArray2D.nSize;
    nLineCount = varArray2D.nLineCount;
    Create();

    int index = 0;
    for (index = 0; index < nSize; ++index) { pData[index] = varArray2D.pData[index]; }
    for (index = 0; index < nLineCount; ++index) { pLineDataSize[index] = varArray2D.pLineDataSize[index]; }
    IndexLineData();
  }

  void Destroy()
  {
    if (pData) { delete[] pData; }
    if (pLineData) { delete[] pLineData; }
    if (pLineDataSize) { delete[] pLineDataSize; }
    nSize = 0;
    nLineCount = 0;
    pData = 0;
    pLineData = 0;
    pLineDataSize = 0;
  }

  void Create()
  {
    pData = new T[nSize];
    pLineData = new PT[nLineCount];
    pLineDataSize = new TCounter[nLineCount];
    for (int index = 0; index < nLineCount; ++index)
    {
      pLineData[index] = 0;
      pLineDataSize[index] = 0;
    }
  }

public:
  CVarArray2D() : nSize(0), nLineCount(0), pData(nullptr), pLineData(nullptr), pLineDataSize(nullptr) {}
  CVarArray2D(int _nSize, int _nLineCount) : nSize(_nSize), nLineCount(_nLineCount) { Create(); }
  CVarArray2D(const CVarArray2D &varArray2D) { Copy(varArray2D); }
  ~CVarArray2D() { Destroy(); }

  const CVarArray2D &operator=(const CVarArray2D &varArray2D)
  {
    if (this == &varArray2D) { return *this; }
    Destroy();
    Copy(varArray2D);
    return *this;
  }

  void SetSizes(int _nSize, int _nLineCount)
  {
    if ((nSize == _nSize) && (nLineCount == _nLineCount)) { return; }
    Destroy();
    NI_ASSERT_T(( _nSize > 0 ) && ( _nLineCount > 0 ),
                NStr::Format( "Can't create 2D var array with zero length (%d) or with zero line count (%d)", _nSize, _nLineCount ));
    nSize = _nSize;
    nLineCount = _nLineCount;
    Create();
  }

  void Clear() { Destroy(); }

  void Set(const T &data)
  {
    for (int index = 0; index < nSize; ++index) { pData[index] = data; }
  }

  T *operator[](const int lineIndex) const
  {
    NI_ASSERT_T(( lineIndex >= 0 ) && ( lineIndex < nLineCount ),
                NStr::Format( "line index (%d) miss in 2D var array line count (%d)", lineIndex, nLineCount ));
    NI_ASSERT_T(pLineData[lineIndex] != 0,
                NStr::Format( "line index (%d) not inisialized in 2D var array line count (%d)", lineIndex, nLineCount ));
    return pLineData[lineIndex];
  }

  T &operator()(const int lineIndex, const TCounter inLineIndex)
  {
    NI_ASSERT_T(( lineIndex >= 0 ) && ( lineIndex < nLineCount ),
                NStr::Format( "line index (%d) miss in 2D var array line count: (%d)", lineIndex, nLineCount ));
    NI_ASSERT_T(( inLineIndex >= 0 ) && ( inLineIndex < pLineDataSize[lineIndex] ),
                NStr::Format( "in line index (%d) miss in 2D var array line  at index (%d): (%d)", inLineIndex, lineIndex, pLineDataSize[lineIndex] ));
    return (*this)[lineIndex][inLineIndex];
  }

  const T &operator()(const int lineIndex, const TCounter inLineIndex) const
  {
    NI_ASSERT_T(( lineIndex >= 0 ) && ( lineIndex < nLineCount ),
                NStr::Format( "line index (%d) miss in 2D var array line count: (%d)", lineIndex, nLineCount ));
    NI_ASSERT_T(( inLineIndex >= 0 ) && ( inLineIndex < pLineDataSize[lineIndex] ),
                NStr::Format( "in line index (%d) miss in 2D var array line  at index (%d): (%d)", inLineIndex, lineIndex, pLineDataSize[lineIndex] ));
    return (*this)[lineIndex][inLineIndex];
  }

  int GetSize() const { return nSize; }

  int GetLineCount() const { return nLineCount; }

  TCounter GetLineSize(int lineIndex) const
  {
    NI_ASSERT_T(( lineIndex >= 0 ) && ( lineIndex < nLineCount ),
                NStr::Format( "line index (%d) miss in 2D var array line count(%d)", lineIndex, nLineCount ));
    NI_ASSERT_T(pLineData[lineIndex] != 0,
                NStr::Format( "line index (%d) not inisialized in 2D var array line count (%d)", lineIndex, nLineCount ));
    return pLineDataSize[lineIndex];
  }

  T *GetBuffer() { return pData; }

  bool IsEmpty() const { return pData == nullptr; }

  // very special function!
  void SetLineLength(int lineIndex, TCounter nLineLength)
  {
#ifdef _DEBUG
    NI_ASSERT_T(( lineIndex >= 0 ) && ( lineIndex < nLineCount ),
                NStr::Format( "line index (%d) miss in 2D var array line count(%d)", lineIndex, nLineCount ));
    int debugIndex = 0;
    int debugCount = 0;
    for (debugIndex = 0; debugIndex < lineIndex; ++debugIndex)
    {
      NI_ASSERT_T(pLineData[debugIndex] != 0,
                  NStr::Format( "line data at (%d) is sero, please set line length at (%d) before (%d)", debugIndex, debugIndex, lineIndex ));
      debugCount += static_cast<int>(pLineDataSize[debugIndex]);
    }
    NI_ASSERT_T(static_cast<int>(nLineLength) <= ( nSize - debugCount ),
                NStr::Format( "line length at (%d) too big: (%d), may be under or equal (%d)", nLineLength, nSize - debugCount ));
#endif // #ifdef_DEBUG

    int index = 0;
    int count = 0;
    for (index = 0; index < lineIndex; index++) { count += static_cast<int>(pLineDataSize[index]); }
    pLineData[lineIndex] = pData + count;
    pLineDataSize[lineIndex] = nLineLength;
  }

  // very special function!
  void SetZero() { memset(pData, 0, sizeof(T) * nSize); }

  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    if (saver.IsReading()) { Destroy(); }
    saver.Add(1, &nSize);
    saver.Add(2, &nLineCount);
    if (saver.IsReading()) { Create(); }
    saver.AddRawData(3, static_cast<void *>(pData), sizeof(T) * nSize);
    saver.AddRawData(4, static_cast<void *>(pLineDataSize), sizeof(TCounter) * nLineCount);
    /* * 
     */
    if (saver.IsReading()) { IndexLineData(); }
    return 0;
  }

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;

    if (saver.IsReading()) { Destroy(); }
    saver.Add("Size", &nSize);
    saver.Add("LineCount", &nLineCount);
    if (saver.IsReading()) { Create(); }
    saver.AddRawData("Data", static_cast<void *>(pData), sizeof(T) * nSize);
    saver.AddRawData("LineData", static_cast<void *>(pLineDataSize), sizeof(TCounter) * nLineCount);
    /* * 
     */
    if (saver.IsReading()) { IndexLineData(); }
    return 0;
  }


  // only under debug

#ifdef _DEBUG
  void Trace(char *pBuffer)
  {
    int elements = 0;
    TCounter maxLineSize = 0;
    int signedLines = 0;
    int lineIndex = 0;
    int index = 0;
    for (lineIndex = 0; lineIndex < GetLineCount(); ++lineIndex)
    {
      for (index = 0; index < GetLineSize(lineIndex); ++index) { ++elements; }
      if (GetLineSize(lineIndex) > maxLineSize) { maxLineSize = GetLineSize(lineIndex); }
      if (GetLineSize(lineIndex) > 0) { ++signedLines; }
    }
    sprintf(pBuffer, "VarArray2D statistic:\nsize %d\nlines count %d\nelements %d\nmax line length %d\nnon-empty lines count %d\naverage elements per line %f\naverage elements per non-emply line %f\n",
            GetSize(),
            GetLineCount(),
            elements,
            maxLineSize,
            signedLines,
            elements * 1.0 / GetLineCount(),
            elements * 1.0 / signedLines);
  }
#endif // #ifdef_DEBUG

};

struct SSpritesPack : CTRefCount<ISharedResource>
{
  OBJECT_SERVICE_METHODS(SSpritesPack);
  DECLARE_SERIALIZE;
  SHARED_RESOURCE_METHODS(nRefData, "ComplexSprite.Animation");

public:
  enum { SIGNATURE = 0x314e4153 };

  // single packed sprite
  struct SSprite
  {
    // single sprite element
    struct SSquare
    {
      CVec2 vLeftTop;// left-top coords (with respect to sprite's center)
      float fSize;// square's size
      CTRect<float> rcMaps;// texture coordinate maps
      float fDepthLeft;// left corner depth
      float fDepthRight;// right corner depth
    };

    // sprite's edge
    struct SEdge
    {
      // edge data
      bool bHorizontal;// is horizontal var array
      CVarArray2D<short, BYTE> edges;// edge - filled per string or per columns (see flag above)
      // bounding box data
      CTRect<float> rcBoundBox;// bounding box ( minx <= minAlpha, miny <= minAlpha, maxx >= minAlpha, maxy >= minAlpha )
      // lines in edges: ( maxx - minx + 1 ) or ( maxy - miny + 1 )
      const CTRect<float> &GetBoundBox() const { return rcBoundBox; }
      // vPos with respect to sprite's center
      const bool IsInside(const CVec2 &vPos) const;
      //
      int operator&(IStructureSaver &ss);
    };

    //
    using CSquaresList = std::vector<SSquare>;
    CSquaresList squares;// all squares
    SEdge edge;// sprite's edge
    CTPoint<int> center;// for working in data composer
    float fMinDepth;// for correct display of icons... calculated when loading data
    //
    const bool IsInside(const CVec2 &vPos) const { return edge.IsInside(vPos); }
    const CTRect<float> &GetBoundBox() const { return edge.GetBoundBox(); }
    //
    int operator&(IStructureSaver &ss);
  };

  //
  using CSpritesList = std::vector<SSprite>;
  CSpritesList sprites;
  //
  void STDCALL SwapData(ISharedResource *pResource) override
  {
    auto pRes = dynamic_cast<SSpritesPack *>(pResource);
    NI_ASSERT_TF(pRes != 0, "shared resource is not a SSpritesPack", return);
    std::swap(sprites, pRes->sprites);
  }

  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
};

//
// SSpritesPack pack;
// DWORD dwSignature = SSpritesPack::SIGNATURE;
// saver.Add( 1, &pack );
// saver.Add( 127, &dwSignature );

#endif // __FMTSPRITE_H__