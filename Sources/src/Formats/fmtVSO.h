#if !defined(__FMT__VSO__H__)
#define __FMT__VSO__H__

#pragma once

#include "../Image/Image.h"

struct SVectorStripeObjectPoint
{
  CVec3 vPos;// point position
  CVec3 vNorm;// normal at this point
  float fRadius;// curvature radius
  float fWidth;// width at this point
  bool bKeyPoint;// key point of the sampling
  float fOpacity;// transparency ( 0..1 ) only for key point

  // ------------------------------------------------------------------------------------------------------------------
  SVectorStripeObjectPoint()
    : vPos(VNULL3), vNorm(VNULL3), fRadius(0.0f), fWidth(0.0f), bKeyPoint(false), fOpacity(1.0f) {}

  // ------------------------------------------------------------------------------------------------------------------
  int operator&(IDataTree &ss);
};


struct SVectorStripeObjectDesc
{
  enum EType
  {
    TYPE_UNKNOUN = 0,
    TYPE_RIVER = 1,
    TYPE_ROAD = 2,
    TYPE_RAILROAD = 3,
  };

  // ------------------------------------------------------------------------------------------------------------------
  struct SLayer
  {
    BYTE opacityCenter;// transparency in the center of the flow
    BYTE opacityBorder;// transparency around the edges
    float fStreamSpeed;// conventional flow rate
    float fTextureStep;// texturing step by tile
    int nNumCells;// width of the stream in cells (in tiles)
    bool bAnimated;// animated layer
    std::string szTexture;// stream texture (or directory if it's an animated texture)
    float fDisturbance;// mesh disturbance
    float fRelWidth;// relative width

    SLayer()
      : opacityCenter(0xff), opacityBorder(0x80), fStreamSpeed(0.1f), fTextureStep(0.1f), nNumCells(4), bAnimated(false), fDisturbance(0.3f), fRelWidth(1) {}

    int operator&(IDataTree &ss);
    int operator&(IStructureSaver &ss);
  };

  // ------------------------------------------------------------------------------------------------------------------
  int eType;// type
  int nPriority;// priority
  float fPassability;// passability
  DWORD dwAIClasses;// AI classes that can't walk this road

  enum ESoilParams
  {
    ESP_TRACE = 0x01,
    ESP_DUST = 0x10
  };

  BYTE cSoilParams;// soil parameters - traces, dust, etc.

  // ------------------------------------------------------------------------------------------------------------------
  // layers
  SLayer bottom;// bottom central layer
  std::vector<SLayer> bottomBorders;// bottom layer border parts
  std::vector<SLayer> layers;// additional layers
  SColor miniMapCenterColor;// color of the object on the minimap (central part)
  SColor miniMapBorderColor;// object color on minimap (edge)

  // ------------------------------------------------------------------------------------------------------------------
  // ambient sound
  std::string szAmbientSound;

  // ------------------------------------------------------------------------------------------------------------------
  SVectorStripeObjectDesc()
    : eType(TYPE_UNKNOUN), nPriority(0), fPassability(1.0f), dwAIClasses(0),
      cSoilParams(0), miniMapCenterColor(0x00000000), miniMapBorderColor(0x00000000) {}

  // ------------------------------------------------------------------------------------------------------------------
  virtual int operator&(IDataTree &ss);
  virtual int operator&(IStructureSaver &ss);
};


struct SVectorStripeObject : SVectorStripeObjectDesc
{
  std::string szDescName;// complete path to descriptor

  // ------------------------------------------------------------------------------------------------------------------
  // points
  std::vector<SVectorStripeObjectPoint> points;// points
  std::vector<CVec3> controlpoints;// control polyline points

  // ------------------------------------------------------------------------------------------------------------------
  // object's ID
  int nID;// ID

  // ------------------------------------------------------------------------------------------------------------------

  int operator&(IDataTree &ss) override;
  int operator&(IStructureSaver &ss) override;
};


using TVSOList = std::vector<SVectorStripeObject>;

#endif // #if !defined(__FMT__VSO__H__)