#if !defined(__WV_Types__)
#define __WV_Types__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Template for creating vectors of objects with weights,
// weights can be any non-negative integers

template<class TYPE>
class CWeightVector
{
  std::vector<TYPE> elements;// elements (must be able to be packed into containers)
  std::vector<int> weights;// weights (the difference between neighbors is the weight of the current element)

public:
  // ------------------------------------------------------------------------------------------------------------------
  // constructors and assignment operators
  CWeightVector() {}
  // ------------------------------------------------------------------------------------------------------------------
  CWeightVector(const CWeightVector &rWeightVector) : elements(rWeightVector.elements), weights(rWeightVector.weights) {}
  // ------------------------------------------------------------------------------------------------------------------
  CWeightVector &operator=(const CWeightVector &rWeightVector)
  {
    if (&rWeightVector != this)
    {
      elements = rWeightVector.elements;
      weights = rWeightVector.weights;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------------------------------
  // accessing elements via operator
  const TYPE &operator[](int nElementIndex) const
  {
    NI_ASSERT_T(( nElementIndex >=0 ) && ( nElementIndex < elements.size() ),
                NStr::Format("Index (%d) miss in SWeightVector (%d)", nElementIndex, elements.size() ));
    return elements[nElementIndex];
  }

  // ------------------------------------------------------------------------------------------------------------------
  TYPE &operator[](int nElementIndex)
  {
    NI_ASSERT_T(( nElementIndex >=0 ) && ( nElementIndex < elements.size() ),
                NStr::Format("Index (%d) miss in SWeightVector (%d)", nElementIndex, elements.size() ));
    return elements[nElementIndex];
  }

  // ------------------------------------------------------------------------------------------------------------------
  // accessing elements through functions
  const TYPE &Get(int nElementIndex) const
  {
    NI_ASSERT_T(( nElementIndex >=0 ) && ( nElementIndex < elements.size() ),
                NStr::Format("Index (%d) miss in SWeightVector (%d)", nElementIndex, elements.size() ));
    return elements[nElementIndex];
  }

  // ------------------------------------------------------------------------------------------------------------------
  void Set(int nElementIndex, const TYPE &rElement)
  {
    NI_ASSERT_T(( nElementIndex >=0 ) && ( nElementIndex < elements.size() ),
                NStr::Format("Index (%d) miss in SWeightVector (%d)", nElementIndex, elements.size() ));
    elements[nElementIndex] = rElement;
  }

  // ------------------------------------------------------------------------------------------------------------------
  int GetWeight(int nElementIndex) const
  {
    NI_ASSERT_T(( nElementIndex >=0 ) && ( nElementIndex < elements.size() ),
                NStr::Format("Index (%d) miss in SWeightVector (%d)", nElementIndex, elements.size() ));
    return (nElementIndex > 0) ? (weights[nElementIndex] - weights[nElementIndex - 1]) : weights[nElementIndex];
  }

  // ------------------------------------------------------------------------------------------------------------------
  void SetWeight(int nElementIndex, int nWeight)
  {
    int nAdditionalWeight = nWeight - GetWeight(nElementIndex);
    for (int nInnerIndex = nElementIndex; nInnerIndex < elements.size(); ++nInnerIndex) { weights[nInnerIndex] += nAdditionalWeight; }
  }

  // ------------------------------------------------------------------------------------------------------------------
  // methods similar to std::vector methods
  void push_back(const TYPE &rElement, int nWeight)
  {
    elements.push_back(rElement);
    if (!weights.empty()) { weights.push_back(weights[weights.size() - 1] + nWeight); }
    else { weights.push_back(nWeight); }
  }

  // ------------------------------------------------------------------------------------------------------------------
  void erase(int nElementIndex)
  {
    int nErasedWeight = GetWeight(nElementIndex);
    for (int nInnerIndex = nElementIndex + 1; nInnerIndex < weights.size(); ++nInnerIndex) { weights[nInnerIndex] -= nErasedWeight; }
    elements.erase(elements.begin() + nElementIndex);
    weights.erase(weights.begin() + nElementIndex);
  }

  // ------------------------------------------------------------------------------------------------------------------
  int size() const { return elements.size(); }
  int weight() const { return !weights.empty() ? weights[weights.size() - 1] : 0; }
  // ------------------------------------------------------------------------------------------------------------------
  void clear()
  {
    elements.clear();
    weights.clear();
  }

  // ------------------------------------------------------------------------------------------------------------------
  bool empty() const { return elements.empty(); }

  // ------------------------------------------------------------------------------------------------------------------
  // get a random element
  int GetRandomIndex(bool bBinarySearch = true) const
  {
    if (weights.empty() || weights[weights.size() - 1] == 0) { return (-1); }

    int nWeight = Random(weights[weights.size() - 1]);
    int nMinIndex = 0;
    int nMaxIndex = weights.size() - 1;

    if (bBinarySearch)
    {
      // binary search:
      while ((nMaxIndex - nMinIndex) > 1)
      {
        int nElementIndex = (nMinIndex + nMaxIndex) / 2;
        if (weights[nElementIndex] > nWeight) { nMaxIndex = nElementIndex; }
        else { nMinIndex = nElementIndex; }
      }
    }
    // simple search
    while (nWeight >= weights[nMinIndex]) ++nMinIndex;
    return nMinIndex;
  }

  // ------------------------------------------------------------------------------------------------------------------
  const TYPE &GetRandom(bool bBinarySearch = true) const
  {
    int nElementIndex = GetRandomIndex(bBinarySearch);
    NI_ASSERT_T(( nElementIndex >=0 ) && ( nElementIndex < elements.size() ),
                NStr::Format("Index (%d) miss in SWeightVector (%d)", nElementIndex, elements.size() ));
    return elements[nElementIndex];
  }

  // ------------------------------------------------------------------------------------------------------------------
  // serializing...
  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;

    saver.Add(1, &elements);
    saver.Add(2, &weights);

    return 0;
  }

  // ------------------------------------------------------------------------------------------------------------------
  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;

    saver.Add("Elements", &elements);
    if (saver.IsReading())
    {
      saver.Add("Weights", &weights);
      for (int nIndex = 1; nIndex < weights.size(); ++nIndex) { weights[nIndex] = weights[nIndex] + weights[nIndex - 1]; }
    }
    else
    {
      std::vector<int> normalWeights;
      if (!weights.empty())
      {
        normalWeights.resize(weights.size());
        normalWeights[0] = weights[0];
        for (int nIndex = 1; nIndex < weights.size(); ++nIndex) { normalWeights[nIndex] = weights[nIndex] - weights[nIndex - 1]; }
      }
      saver.Add("Weights", &normalWeights);
    }
    return 0;
  }
};

using TStringWeightVector = CWeightVector<std::string>;
using TIntWeightVector = CWeightVector<int>;

#endif // !defined(__WV_Types__)