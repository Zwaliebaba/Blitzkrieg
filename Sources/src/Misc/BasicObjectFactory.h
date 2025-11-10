#ifndef __BASICOBJECTFACTORY_H__
#define __BASICOBJECTFACTORY_H__

#pragma once

class CBasicObjectFactory : public IObjectFactory
{
  using CNewFuncsMap = std::hash_map<int, ObjectFactoryNewFunc>;
  using CRTTIMap = std::hash_map<const type_info *, int, SDefaultPtrHash>;
  // table of functions for creating new objects
  CNewFuncsMap newfuncs;
  CRTTIMap rttis;
  // internal¤¤ registration
  void RegisterType(int nObjectTypeID, const type_info *pObjectTypeInfo, ObjectFactoryNewFunc newFunc);

public:
  // create an object by its typeID
  IRefCount * STDCALL CreateObject(int nTypeID) override;
  // register type
  void STDCALL RegisterType(int nObjectTypeID, ObjectFactoryNewFunc newFunc) override;
  // aggregate another factory inside this one (re-register its objects to this factory)
  void STDCALL Aggregate(IObjectFactory *pFactory) override;
  // get the number of object types that this factory (+ all aggregated in it) can create
  int STDCALL GetNumKnownTypes() override { return newfuncs.size(); }
  // get type info of objects that this factory (+ all aggregated in it) can create
  void STDCALL GetKnownTypes(SObjectFactoryTypeInfo *pInfoBuffer, int nBufferSize) override;
  // get the typeID of an object by a pointer to it
  int STDCALL GetObjectTypeID(IRefCount *pObj) const override
  {
    NI_ASSERT_T(pObj != 0, "can't get object type ID from NULL pointer");
    const auto &rtti = typeid(*pObj);
    auto pos = rttis.find(&rtti);
    return pos != rttis.end() ? pos->second : -1;
  }
};

#define REGISTER_CLASS( pFactory, nTypeID, className ) pFactory->RegisterType( nTypeID, reinterpret_cast<ObjectFactoryNewFunc>( className##::CreateNewClassInstanceInternal ) );

#endif // __BASICOBJECTFACTORY_H__