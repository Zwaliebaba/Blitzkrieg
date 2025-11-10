#ifndef __MANIPULATOR_H__
#define __MANIPULATOR_H__

#pragma once

struct SBaseProperty : IRefCount
{
  enum ENodeType { LEAF, SINGLE, VECTOR };

  ENodeType nodeType;// property node type: leaf, single manipulator, vector of the manipulators
  int nType;// single property type (int, float, string, etc.)

  std::vector<variant_t> values;
  SPropertyDesc::EPropertyType ePropType;// property type
  //
  virtual bool Set(IManipulator *pManipulator, const variant_t &value) = 0;
  virtual bool Get(IManipulator *pManipulator, variant_t *pValue, int nIndex = -1) = 0;

  SBaseProperty() : nodeType(LEAF), nType(0) {}

};

template<class TYPE>
struct SProperty : SBaseProperty
{
  OBJECT_MINIMAL_METHODS(SProperty);

public:
  //
  using SETFUNCTION = void(TYPE::*)(const variant_t &value);
  using GETFUNCTION = void(TYPE::*)(variant_t *pValue, int nIndex);
  SETFUNCTION pfnSet;// set property function
  GETFUNCTION pfnGet;// get property function
  //
  SProperty() : pfnSet(nullptr), pfnGet(nullptr) {}

  //
  bool Set(IManipulator *pManipulator, const variant_t &value) override
  {
    if (!pfnSet) return false;
    TYPE *pMan = dynamic_cast<TYPE *>(pManipulator);
    (pMan->*pfnSet)(value);
    return true;
  }

  bool Get(IManipulator *pManipulator, variant_t *pValue, int nIndex = -1) override
  {
    if (!pfnGet) return false;
    TYPE *pMan = dynamic_cast<TYPE *>(pManipulator);
    (pMan->*pfnGet)(pValue, nIndex);
    return true;
  }
};

namespace NProperty
{

  // internal property descriptor
  struct SPropDesc
  {
    std::string szName;
    CPtr<SBaseProperty> pProperty;
    //
    SPropDesc() {}
    SPropDesc(const std::string &_szName, SBaseProperty *_pProperty) : szName(_szName), pProperty(_pProperty) {}
  };

  // properties set
  struct SProperties
  {
    using CPropertiesMap = std::hash_map<std::string, CPtr<SBaseProperty>>;
    using CPropertiesList = std::list<SPropDesc>;
    CPropertiesMap propMap;// properties map for a fast access
    CPropertiesList propList;// properties list for a sequential access
    //
    using iterator = CPropertiesList::iterator;
    using const_iterator = CPropertiesList::const_iterator;
    //
    SBaseProperty *GetProperty(const std::string &szName)
    {
      CPropertiesMap::iterator pos = propMap.find(szName);
      return pos != propMap.end() ? pos->second.GetPtr() : nullptr;
    }

    const SBaseProperty *GetProperty(const std::string &szName) const
    {
      CPropertiesMap::const_iterator pos = propMap.find(szName);
      return pos != propMap.end() ? pos->second.GetPtr() : nullptr;
    }

    // iterating
    iterator begin() { return propList.begin(); }
    iterator end() { return propList.end(); }
    const_iterator begin() const { return propList.begin(); }
    const_iterator end() const { return propList.end(); }
  };

};

class CPropertiesRegister
{
  using CRegistersMap = std::hash_map<std::string, NProperty::SProperties>;
  CRegistersMap registers;

public:
  bool HasRegister(const std::string &szRegister) const { return registers.find(szRegister) != registers.end(); }
  bool AddProperty(const std::string &szRegister, const std::string &szName, SBaseProperty *pPropertyDesc);
  SBaseProperty *GetProperty(const std::string &szRegister, const std::string &szName);
  // get entire properties set
  NProperty::SProperties *GetProperties(const std::string &szRegister)
  {
    CRegistersMap::iterator pos = registers.find(szRegister);
    return pos != registers.end() ? &(pos->second) : nullptr;
  }
};

// ************************************************************************************************************************ //
// **
// ** manipulator and iterator
// **
// **
// **
// ************************************************************************************************************************ //

class CManipulator : public IManipulator
{
  friend class CManipulatorIterator;
  //
  CPropertiesRegister *pRegister;// reference to the properties register
  std::string szRegister;// register entry name for this manipulator
  // enumeration data
  SPropertyDesc tempPropDesc;// temporal prop desc for returing
  //
  const SPropertyDesc *FillTempProps(const char *pszName, const SBaseProperty *pProp);

protected:
  bool DoWeNeedFillProps() const;
  SBaseProperty *GetProperty(const std::string &szName) { return pRegister->GetProperty(szRegister, szName); }
  const NProperty::SProperties *GetAllProperties() { return pRegister->GetProperties(szRegister); }
  bool AddProperty(const std::string &szName, SBaseProperty *pPropertyDesc) { return pRegister->AddProperty(szRegister, szName, pPropertyDesc); }

  template<class TYPE>
  SProperty<TYPE> *AddNewProperty(TYPE *, const std::string &szName, void (TYPE::*pfnSet)(const variant_t &value),
                                  void (TYPE::*pfnGet)(variant_t *pValue, int nIndex),
                                  SPropertyDesc::EPropertyType propType, SBaseProperty::ENodeType nodeType)
  {
    SProperty<TYPE> *pProp = new SProperty<TYPE>();
    pProp->pfnSet = pfnSet;
    pProp->pfnGet = pfnGet;
    pProp->nodeType = nodeType;
    pProp->ePropType = propType;
    if (pRegister->AddProperty(szRegister, szName, pProp) == false)
    {
      delete pProp;
      pProp = 0;
    }
    return pProp;
  }

  //
  IManipulator *GetPropertyAsManipulator(SBaseProperty *pProp, int nIndex = -1);
  SBaseProperty *GetProperty(const std::string &szFullName, std::string *pRestName, int *pnIndex);

public:
  CManipulator(CPropertiesRegister *_pRegister, const char *pszRegisterName)
    : pRegister(_pRegister), szRegister(pszRegisterName) {}

  //
  IManipulatorIterator * STDCALL Iterate() override;
  const SPropertyDesc * STDCALL GetPropertyDesc(const char *pszName) override;
  bool STDCALL GetValue(const char *pszValueName, variant_t *pValue) override;
  bool STDCALL SetValue(const char *pszValueName, const variant_t &value) override;
};

class CManipulatorIterator : public IManipulatorIterator
{
  OBJECT_MINIMAL_METHODS(CManipulatorIterator);
  //
  CPtr<CManipulator> pManipulator;// base manipulator to iterate
  const NProperty::SProperties *pProperties;// this iterator properties set
  NProperty::SProperties::const_iterator itProps;// properties iterator
  // local variable to use during enumeration
  mutable SBaseProperty *pPropLocal;// current local properties
  CPtr<IManipulatorIterator> pItLocal;// current iterator during iteration inside the complex property
  int nPropIndexLocal;// current manipulator in the complex property
  mutable std::string szBaseName;// base property name
  mutable std::string szPropName;// local structure to store property name for descriptor
  mutable SPropertyDesc propDesc;// local structure to store property descriptor
  const SPropertyDesc *pDescLocal;

protected:
  const SPropertyDesc *GetFirst();
  const SPropertyDesc *GetNext();
  //
  const SPropertyDesc *GetNextProperty();
  const SPropertyDesc *GetFirstProperty();
  //
  bool GetNextLocal();
  //
  void FillLocals() const
  {
    pPropLocal = const_cast<SBaseProperty *>(pProperties->GetProperty(itProps->szName));
    szBaseName = itProps->szName.c_str();
  }

public:
  CManipulatorIterator(CManipulator *_pManipulator)
    : pManipulator(_pManipulator), pPropLocal(nullptr), nPropIndexLocal(-1)
  {
    pProperties = nullptr;
    if (pManipulator)
    {
      pProperties = pManipulator->GetAllProperties();
      itProps = pProperties->begin();
      if (itProps != pProperties->end()) FillLocals();
      pDescLocal = GetFirstProperty();
    }
  }

  bool STDCALL Next() override
  {
    pDescLocal = GetNextProperty();
    return !IsEnd();
  }

  bool STDCALL IsEnd() const override { return (itProps == pProperties->end()) || (pDescLocal == nullptr); }

  const SPropertyDesc * STDCALL GetPropertyDesc() const override
  {
    if ((pProperties == nullptr) || (itProps == pProperties->end())) return nullptr;
    //
    return pDescLocal;
  }
};

// ************************************************************************************************************************ //
// **
// ** multimanipulator
// **
// **
// **
// ************************************************************************************************************************ //

struct SMultiManipulatorProperty
{
  std::string szName;
  int nType;
  SPropertyDesc::EPropertyType ePropType;
  std::vector<variant_t> values;
  //
  int nCounter;
  int nOrderIndex;
  //
  SMultiManipulatorProperty()
    : nCounter(0), nOrderIndex(0) {}
};

class CMultiManipulator : public IMultiManipulator
{
  OBJECT_MINIMAL_METHODS(CMultiManipulator);
  friend class CMultiManipulatorIterator;
  //
  using CManipulatorsList = std::vector<CPtr<IManipulator>>;
  CManipulatorsList manipulators;
  using CPropsMap = std::hash_map<std::string, SMultiManipulatorProperty>;
  CPropsMap propsMap;
  bool bPropsAlreadyBuilt;
  //
  using CPropsList = std::list<SMultiManipulatorProperty *>;
  CPropsList propsList;
  //
  void BuildProps();

public:
  CMultiManipulator()
    : bPropsAlreadyBuilt(false) {}

  // begin to iterate through all properties
  IManipulatorIterator * STDCALL Iterate() override;
  // get property descriptor by name
  const SPropertyDesc * STDCALL GetPropertyDesc(const char *pszName) override
  {
    // CRAP { needs to be done differently...
    CPropsMap::const_iterator pos = propsMap.find(pszName);
    if ((pos == propsMap.end()) || (pos->second.nCounter != manipulators.size()) || manipulators.empty()) return nullptr;
    return manipulators[0]->GetPropertyDesc(pszName);
    // CRAP}
  }

  // retrieve value. 
  bool STDCALL GetValue(const char *pszValueName, variant_t *pValue) override;
  // set value. 
  bool STDCALL SetValue(const char *pszValueName, const variant_t &value) override
  {
    for (auto it = manipulators.begin(); it != manipulators.end(); ++it) (*it)->SetValue(pszValueName, value);
    return true;
  }

  // remove all manipulators
  void STDCALL Clear() override
  {
    manipulators.clear();
    propsMap.clear();
  }

  // add new manipulator
  void STDCALL AddManipulator(IManipulator *pMan) override;
};

class CMultiManipulatorIterator : public IManipulatorIterator
{
  OBJECT_MINIMAL_METHODS(CMultiManipulatorIterator);
  //
  CPtr<CMultiManipulator> pManipulator;// base manipulator to iterate
  CMultiManipulator::CPropsList::iterator itProp;// current iterator
  mutable SPropertyDesc propDesc;// temp variable
public:
  CMultiManipulatorIterator(CMultiManipulator *_pManipulator)
    : pManipulator(_pManipulator) { if (pManipulator) itProp = pManipulator->propsList.begin(); }

  bool STDCALL Next() override
  {
    if ((pManipulator == nullptr) || (itProp == pManipulator->propsList.end())) return false;
    ++itProp;
    return !IsEnd();
  }

  bool STDCALL IsEnd() const override { return itProp == pManipulator->propsList.end(); }

  const SPropertyDesc * STDCALL GetPropertyDesc() const override
  {
    propDesc.pszName = (*itProp)->szName.c_str();
    propDesc.ePropType = (*itProp)->ePropType;
    propDesc.values = (*itProp)->values;
    return &propDesc;
  }
};

#define BEGIN_PROPERTIES_MAP( classname )	\
	if ( DoWeNeedFillProps() )							\
	{																				\
		typedef SProperty<##classname##Manipulator> ##classname##Property;

#define END_PROPERTIES_MAP }

#endif		// __MANIPULATOR_H__