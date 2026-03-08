#include "StdAfx.h"

#include "InputObjectFactory.h"

#include "InputBinder.h"
#include "InputBind.h"
#include "InputSlider.h"

static CInputObjectFactory theInputObjectFactory;

CInputObjectFactory::CInputObjectFactory()
{
  REGISTER_CLASS(this, INPUT_INPUT, CInputBinder);
  REGISTER_CLASS(this, INPUT_BIND, CInputBind);
  REGISTER_CLASS(this, INPUT_SLIDER, CInputSlider);
}

IObjectFactory * STDCALL GetInputObjectFactory() { return &theInputObjectFactory; }