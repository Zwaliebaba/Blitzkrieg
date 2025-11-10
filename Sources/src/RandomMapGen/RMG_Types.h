#ifndef __RMG_TYPES_H__
#define __RMG_TYPES_H__

#include "../Formats/FmtMap.h"
#include "WV_Types.h"

// string constants used to work with xml files
extern const char *RMGC_CONTAINER_XML_NAME;
extern const char *RMGC_GRAPH_XML_NAME;
extern const char *RMGC_OBJECTSET_XML_NAME;
extern const char *RMGC_TILESET_XML_NAME;
extern const char *RMGC_TEMPLATE_XML_NAME;
extern const char *RMGC_FIELDSET_XML_NAME;

extern const char *RMGC_ROAD_LEVEL_FILE_NAME;
extern const char *RMGC_RIVER_LEVEL_FILE_NAME;
extern const char *RMGC_RM_LEVEL_VSO_PARAMETER_NAME;
extern const char *RMGC_CONTEXT_NAME;
extern const char *RMGC_SETTING_NAME;
extern const char *RMGC_SETTING_DEFAULT_FOLDER;
extern const char *RMGC_ANY_SETTING_NAME;
extern const char *RMGC_NO_MOD_FOLDER;
extern const char *RMGC_CURRENT_MOD_FOLDER;
extern const char *RMGC_OWN_MOD_FOLDER;

extern const int RMGC_CREATE_RANDOM_MAP_STEP_COUNT;
extern const int RMGC_CREATE_MINIMAP_IMAGE_STEP_COUNT;

using CUsedLinkIDs = std::set<int>;
using CUsedScriptIDs = std::set<int>;
using CUsedScriptAreas = std::set<std::string>;

struct SRMLevelVSOParameter
{
  std::string szProfileFileName;// profile name (if empty, then we don’t raise or lower it, it doesn’t do anything, i.e.)
  float fHeight;// maximum profile height (if zero, then nothing happens)
  bool bAdd;// raise/lower
  int nMiddlePointsCount;// raise/lower
  bool bLevelEnds;// leveling the ends of rivers and roads
  bool bLevelPatches;// align patches

  // constructors
  SRMLevelVSOParameter() : fHeight(0.0f), bAdd(true), nMiddlePointsCount(0), bLevelEnds(false), bLevelPatches(true) {}
  SRMLevelVSOParameter(const SRMLevelVSOParameter &rRMLevelVSOParameter) : szProfileFileName(rRMLevelVSOParameter.szProfileFileName), fHeight(rRMLevelVSOParameter.fHeight), bAdd(rRMLevelVSOParameter.bAdd), nMiddlePointsCount(rRMLevelVSOParameter.nMiddlePointsCount), bLevelEnds(rRMLevelVSOParameter.bLevelEnds), bLevelPatches(rRMLevelVSOParameter.bLevelPatches) {}

  SRMLevelVSOParameter &operator=(const SRMLevelVSOParameter &rRMLevelVSOParameter)
  {
    if (&rRMLevelVSOParameter != this)
    {
      szProfileFileName = rRMLevelVSOParameter.szProfileFileName;
      fHeight = rRMLevelVSOParameter.fHeight;
      bAdd = rRMLevelVSOParameter.bAdd;
      nMiddlePointsCount = rRMLevelVSOParameter.nMiddlePointsCount;
      bLevelEnds = rRMLevelVSOParameter.bLevelEnds;
      bLevelPatches = rRMLevelVSOParameter.bLevelPatches;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


struct SRMPatch
{
  CTPoint<int> size;// patch size in VIS patches (16 by 16 VIS tiles)
  std::string szFileName;// card name
  std::string szPlace;// locality name

  // constructors
  SRMPatch() : size(0, 0) {}
  SRMPatch(const CTPoint<int> &rSize, const std::string &rszFileName, const std::string &rszPlace) : size(rSize), szFileName(rszFileName), szPlace(rszPlace) {}
  SRMPatch(const SRMPatch &rPatch) : size(rPatch.size), szFileName(rPatch.szFileName), szPlace(rPatch.szPlace) {}

  SRMPatch &operator=(const SRMPatch &rPatch)
  {
    if (&rPatch != this)
    {
      size = rPatch.size;
      szFileName = rPatch.szFileName;
      szPlace = rPatch.szPlace;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};

using CRMPatchesList = std::vector<SRMPatch>;

struct SRMContainer
{
  enum ANGLE
  {
    ANGLE_0 = 0,
    ANGLE_90 = 1,
    ANGLE_180 = 2,
    ANGLE_270 = 3,
  };

  enum DIRECTION
  {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
  };

  CRMPatchesList patches;// patch set
  std::vector<int> indices[4];// patch indices for each of the turns in the previous vector

  // check info
  CTPoint<int> size;// size in VIS patches (16 by 16 VIS tiles)
  int nSeason;// season
  std::string szSeasonFolder;// season catalog
  CUsedScriptIDs usedScriptIDs;// scriptIDs used
  CUsedScriptAreas usedScriptAreas;// used scriptAreas

  // constructors
  SRMContainer() : size(0, 0), nSeason(0) {}

  SRMContainer(const SRMContainer &rContainer)
    : patches(rContainer.patches),
      size(rContainer.size),
      nSeason(rContainer.nSeason),
      szSeasonFolder(rContainer.szSeasonFolder),
      usedScriptIDs(rContainer.usedScriptIDs),
      usedScriptAreas(rContainer.usedScriptAreas)
  {
    for (int index = 0; index < 4; ++index) { indices[index] = rContainer.indices[index]; }
  }

  SRMContainer &operator=(const SRMContainer &rContainer)
  {
    if (&rContainer != this)
    {
      patches = rContainer.patches;
      size = rContainer.size;
      nSeason = rContainer.nSeason;
      szSeasonFolder = rContainer.szSeasonFolder;
      usedScriptIDs = rContainer.usedScriptIDs;
      usedScriptAreas = rContainer.usedScriptAreas;

      for (int index = 0; index < 4; ++index) { indices[index] = rContainer.indices[index]; }
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);

  int GetIndices(int nDirection, const std::string &rszPlace, std::vector<int> *pIndices) const;
  int GetSupportedSettings(std::list<std::string> *pSupportedSettingsList) const;
  bool IsSupportedSetting(const std::string &rszSettingName) const;
};


using CRMContainersHashMap = std::hash_map<std::string, SRMContainer>;


struct SRMGraphNode
{
  CTRect<int> rect;// position in VIS tiles in AI coordinates (inverted relative to terrainY)
  std::string szContainerFileName;// container path

  // constructors
  SRMGraphNode() : rect(0, 0, 0, 0) {}
  SRMGraphNode(const SRMGraphNode &rGraphNode) : rect(rGraphNode.rect), szContainerFileName(rGraphNode.szContainerFileName) {}

  SRMGraphNode &operator=(const SRMGraphNode &rGraphNode)
  {
    if (&rGraphNode != this)
    {
      rect = rGraphNode.rect;
      szContainerFileName = rGraphNode.szContainerFileName;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


struct SRMGraphLink
{
  static const int INVALID_LINK_VALUE;
  static const float DEFAULT_RADIUS;
  static const int DEFAULT_PARTS;
  static const float DEFAULT_MIN_LENGTH;
  static const float DEFAULT_DISTANCE;
  static const float DEFAULT_DISTURBANCE;

  enum TYPE
  {
    TYPE_ROAD = 0,
    TYPE_RIVER = 1,
  };

  CTPoint<int> link;// node numbers (set in the template)
  int nType;// type VSO (river, road)
  std::string szDescFileName;// name VSO

  float fRadius;// radius of curvature
  int nParts;// number of elements on the circle
  float fMinLength;// the minimum distance between points that is not subject to crushing
  float fDistance;// distance from edges to possible deviation ( 0...1 )
  float fDisturbance;// deviation ( 0...1 )

  // constructors
  SRMGraphLink() : link(INVALID_LINK_VALUE, INVALID_LINK_VALUE), nType(TYPE_ROAD), fRadius(DEFAULT_RADIUS), nParts(DEFAULT_PARTS), fMinLength(DEFAULT_MIN_LENGTH), fDistance(DEFAULT_DISTANCE), fDisturbance(DEFAULT_DISTURBANCE) {}

  SRMGraphLink(const SRMGraphLink &rGraphLink)
    : link(rGraphLink.link),
      nType(rGraphLink.nType),
      szDescFileName(rGraphLink.szDescFileName),
      fRadius(rGraphLink.fRadius),
      nParts(rGraphLink.nParts),
      fMinLength(rGraphLink.fMinLength),
      fDistance(rGraphLink.fDistance),
      fDisturbance(rGraphLink.fDisturbance) {}

  SRMGraphLink &operator=(const SRMGraphLink &rGraphLink)
  {
    if (&rGraphLink != this)
    {
      link = rGraphLink.link;
      nType = rGraphLink.nType;
      szDescFileName = rGraphLink.szDescFileName;
      fRadius = rGraphLink.fRadius;
      nParts = rGraphLink.nParts;
      fMinLength = rGraphLink.fMinLength;
      fDistance = rGraphLink.fDistance;
      fDisturbance = rGraphLink.fDisturbance;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


using CRMGraphNodesList = std::vector<SRMGraphNode>;
using CRMGraphLinksList = std::vector<SRMGraphLink>;


struct SRMGraph
{
  CRMGraphNodesList nodes;
  CRMGraphLinksList links;

  // check info
  CTPoint<int> size;// in size in VIS patches (16 by 16 VIS tiles)
  int nSeason;// season
  std::string szSeasonFolder;// season catalog
  CUsedScriptIDs usedScriptIDs;// used scriptIDs
  CUsedScriptAreas usedScriptAreas;// used scriptAreas

  // constructors
  SRMGraph() : size(0, 0), nSeason(0) {}

  SRMGraph(const SRMGraph &rGraph)
    : nodes(rGraph.nodes),
      links(rGraph.links),
      size(rGraph.size),
      nSeason(rGraph.nSeason),
      szSeasonFolder(rGraph.szSeasonFolder),
      usedScriptIDs(rGraph.usedScriptIDs),
      usedScriptAreas(rGraph.usedScriptAreas) {}

  SRMGraph &operator=(const SRMGraph &rGraph)
  {
    if (&rGraph != this)
    {
      nodes = rGraph.nodes;
      links = rGraph.links;
      size = rGraph.size;
      nSeason = rGraph.nSeason;
      szSeasonFolder = rGraph.szSeasonFolder;
      usedScriptIDs = rGraph.usedScriptIDs;
      usedScriptAreas = rGraph.usedScriptAreas;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);

  int GetSupportedSettings(std::list<std::string> *pSupportedSettingsList) const;
  bool IsSupportedSetting(const std::string &rszSettingName) const;
};

using CRMGraphsHashMap = std::hash_map<std::string, SRMGraph>;
using CRMGraphWeightVector = CWeightVector<std::string>;


struct SRMObjectSetShell
{
  TStringWeightVector objects;
  float fWidth;// VIS tiles
  int nBetweenDistance;// VIS tiles
  float fRatio;// (0...1)

  // constructors
  SRMObjectSetShell() : fWidth(0.0f), nBetweenDistance(0), fRatio(0) {}
  SRMObjectSetShell(const SRMObjectSetShell &rObjectSetShell) : objects(rObjectSetShell.objects), fWidth(rObjectSetShell.fWidth), nBetweenDistance(rObjectSetShell.nBetweenDistance), fRatio(rObjectSetShell.fRatio) {}

  SRMObjectSetShell &operator=(const SRMObjectSetShell &rObjectSetShell)
  {
    if (&rObjectSetShell != this)
    {
      objects = rObjectSetShell.objects;
      fWidth = rObjectSetShell.fWidth;
      nBetweenDistance = rObjectSetShell.nBetweenDistance;
      fRatio = rObjectSetShell.fRatio;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


struct SRMTileSetShell
{
  TIntWeightVector tiles;
  float fWidth;// VIS tiles

  // constructors
  SRMTileSetShell() : fWidth(0.0f) {}
  SRMTileSetShell(const SRMTileSetShell &rTileSetShell) : tiles(rTileSetShell.tiles), fWidth(rTileSetShell.fWidth) {}

  SRMTileSetShell &operator=(const SRMTileSetShell &rTileSetShell)
  {
    if (&rTileSetShell != this)
    {
      tiles = rTileSetShell.tiles;
      fWidth = rTileSetShell.fWidth;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


using CRMObjectSet = std::vector<SRMObjectSetShell>;
using CRMTileSet = std::vector<SRMTileSetShell>;
using CRMObjectSetHashMap = std::hash_map<std::string, CRMObjectSet>;
using CRMTileSetHashMap = std::hash_map<std::string, CRMTileSet>;


struct SRMFieldSet
{
  CRMObjectSet objectsShells;// trees
  CRMTileSet tilesShells;// Earth

  std::string szProfileFileName;// profile name (if empty, then we do not raise the ground)
  float fHeight;// maximum profile height (if zero, then nothing happens)
  CTPoint<int> patternSize;// minimum - maximum half-size of the pattern
  float fPositiveRatio;// 1 - raise everything, 0 - lower everything

  // only for composers
  int nSeason;// season (summer/winter/Africa:)
  std::string szSeasonFolder;// season catalog

  void ValidateFieldSet(const struct STilesetDesc &rTilesetDesc, int nDefaultTileIndex);

  // constructors
  SRMFieldSet() : fHeight(4.0f), patternSize(2, 5), fPositiveRatio(0.5f), nSeason(0) {}

  SRMFieldSet(const SRMFieldSet &rFieldSet)
    : objectsShells(rFieldSet.objectsShells),
      tilesShells(rFieldSet.tilesShells),
      szProfileFileName(rFieldSet.szProfileFileName),
      fHeight(rFieldSet.fHeight),
      patternSize(rFieldSet.patternSize),
      fPositiveRatio(rFieldSet.fPositiveRatio),
      nSeason(rFieldSet.nSeason),
      szSeasonFolder(rFieldSet.szSeasonFolder) {}

  SRMFieldSet &operator=(const SRMFieldSet &rFieldSet)
  {
    if (&rFieldSet != this)
    {
      objectsShells = rFieldSet.objectsShells;
      tilesShells = rFieldSet.tilesShells;
      szProfileFileName = rFieldSet.szProfileFileName;
      fHeight = rFieldSet.fHeight;
      patternSize = rFieldSet.patternSize;
      fPositiveRatio = rFieldSet.fPositiveRatio;
      nSeason = rFieldSet.nSeason;
      szSeasonFolder = rFieldSet.szSeasonFolder;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};

using CRMFieldSetsHashMap = std::hash_map<std::string, SRMFieldSet>;


struct SRMVSODesc
{
  static const float DEFAULT_WIDTH;
  static const float DEFAULT_OPACITY;

  std::string szVSODescFileName;
  float fWidth;
  float fOpacity;

  // constructors
  SRMVSODesc() : fWidth(DEFAULT_WIDTH), fOpacity(DEFAULT_OPACITY) {}
  SRMVSODesc(const SRMVSODesc &rVSODesc) : szVSODescFileName(rVSODesc.szVSODescFileName), fWidth(rVSODesc.fWidth), fOpacity(rVSODesc.fOpacity) {}

  SRMVSODesc &operator=(const SRMVSODesc &rVSODesc)
  {
    if (&rVSODesc != this)
    {
      szVSODescFileName = rVSODesc.szVSODescFileName;
      fWidth = rVSODesc.fWidth;
      fOpacity = rVSODesc.fOpacity;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


using CRMFieldSetWeightVector = CWeightVector<std::string>;
using CRMVSOWeightVector = CWeightVector<SRMVSODesc>;

struct SRMTemplate
{
  CRMFieldSetWeightVector fields;// Information on filling empty spaces
  CRMGraphWeightVector graphs;// graphs of the location of patches and connections between them
  CRMVSOWeightVector vso;// intermediate roads

  // check info
  CTPoint<int> size;// in size in VIS patches (16 by 16 VIS tiles)
  int nSeason;// season (summer/winter/Africa:)
  std::string szSeasonFolder;// season catalog
  std::string szPlace;// reserved
  CUsedScriptIDs usedScriptIDs;// scriptIDs used
  CUsedScriptAreas usedScriptAreas;// used scriptAreas

  int nDefaultFieldIndex;// default field (default 0)

  std::string szScriptFile;// file with mission's scripts
  CVec3 vCameraAnchor;// camera start position
  std::vector<BYTE> diplomacies;// diplomacy, 0, 1 - warring parties, 2 - neutrals
  SUnitCreationInfo unitCreation;// Each player on the map has its own set of parameters for aviation and other units
  std::string szForestCircleSounds;// to create forest sounds
  std::string szForestAmbientSounds;// to create forest sounds

  std::string szChapterName;// chapter name
  int nMissionIndex;// mission number in this chapter
  int nType;// map type (see CMapInfo)
  int nAttackingSide;// attacking side (for multiplayer (0 - 1))

  // MOD Support
  std::string szMODName;
  std::string szMODVersion;


  // constructors
  SRMTemplate() : size(0, 0), nSeason(0), nDefaultFieldIndex(-1), vCameraAnchor(VNULL3), nMissionIndex(0), nType(0), nAttackingSide(0) { FillDefaultDiplomacies(); }

  SRMTemplate(const SRMTemplate &rTemplate)
    : fields(rTemplate.fields),
      graphs(rTemplate.graphs),
      vso(rTemplate.vso),
      size(rTemplate.size),
      nSeason(rTemplate.nSeason),
      szSeasonFolder(rTemplate.szSeasonFolder),
      szPlace(rTemplate.szPlace),
      usedScriptIDs(rTemplate.usedScriptIDs),
      usedScriptAreas(rTemplate.usedScriptAreas),
      nDefaultFieldIndex(rTemplate.nDefaultFieldIndex),
      szScriptFile(rTemplate.szScriptFile),
      vCameraAnchor(rTemplate.vCameraAnchor),
      diplomacies(rTemplate.diplomacies),
      unitCreation(rTemplate.unitCreation),
      szForestCircleSounds(rTemplate.szForestCircleSounds),
      szForestAmbientSounds(rTemplate.szForestAmbientSounds),
      szChapterName(rTemplate.szChapterName),
      nMissionIndex(rTemplate.nMissionIndex),
      nType(rTemplate.nType),
      nAttackingSide(rTemplate.nAttackingSide),
      szMODName(rTemplate.szMODName),
      szMODVersion(rTemplate.szMODVersion) {}

  SRMTemplate &operator=(const SRMTemplate &rTemplate)
  {
    if (&rTemplate != this)
    {
      fields = rTemplate.fields;
      graphs = rTemplate.graphs;
      vso = rTemplate.vso;
      size = rTemplate.size;
      nSeason = rTemplate.nSeason;
      szSeasonFolder = rTemplate.szSeasonFolder;
      szPlace = rTemplate.szPlace;
      usedScriptIDs = rTemplate.usedScriptIDs;
      usedScriptAreas = rTemplate.usedScriptAreas;
      nDefaultFieldIndex = rTemplate.nDefaultFieldIndex;
      szScriptFile = rTemplate.szScriptFile;
      vCameraAnchor = rTemplate.vCameraAnchor;
      diplomacies = rTemplate.diplomacies;
      unitCreation = rTemplate.unitCreation;
      szForestCircleSounds = rTemplate.szForestCircleSounds;
      szForestAmbientSounds = rTemplate.szForestAmbientSounds;
      szChapterName = rTemplate.szChapterName;
      nMissionIndex = rTemplate.nMissionIndex;
      nType = rTemplate.nType;
      nAttackingSide = rTemplate.nAttackingSide;
      szMODName = rTemplate.szMODName;
      szMODVersion = rTemplate.szMODVersion;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);

  void FillDefaultDiplomacies();

  //
  int GetSupportedSettings(std::list<std::string> *pSupportedSettingsList) const;
  bool IsSupportedSetting(const std::string &rszSettingName) const;
};

using CRMTemplatesHashMap = std::hash_map<std::string, SRMTemplate>;


// for building roads
struct SRMPlacedPatch : SRMPatch
{
  enum EPointSelectionType
  {
    PST_TWO = 0,
    PST_ONE = 1,
    PST_ANY = 2,
    PST_EMPTY = 3,
  };

  // road search structure
  struct SVSOPoint
  {
    int nID;
    bool bBegin;
    CVec2 vPos;
    std::string szVSODescFileName;

    SVSOPoint() : nID(-1), bBegin(true), vPos(VNULL2) {}
    SVSOPoint(const SVSOPoint &rVSOPoint) : nID(rVSOPoint.nID), bBegin(rVSOPoint.bBegin), vPos(rVSOPoint.vPos), szVSODescFileName(rVSOPoint.szVSODescFileName) {}

    SVSOPoint &operator=(const SVSOPoint &rVSOPoint)
    {
      if (&rVSOPoint != this)
      {
        nID = rVSOPoint.nID;
        bBegin = rVSOPoint.bBegin;
        vPos = rVSOPoint.vPos;
        szVSODescFileName = rVSOPoint.szVSODescFileName;
      }
      return *this;
    }
  };

  // general data
  CTPoint<int> minXYCorner;// position of the patch in VIS tiles in AI coordinates (inverted relative to terrainY )

  // road search data
  std::list<SVSOPoint> riversPoints;// river junction points
  std::list<SVSOPoint> roadsPoints;// road junction points

  // constructors
  SRMPlacedPatch() : minXYCorner(0, 0) {}
  SRMPlacedPatch(const SRMPatch &rPatch) : SRMPatch(rPatch), minXYCorner(0, 0) {}
  SRMPlacedPatch(const SRMPlacedPatch &rPlacedPatch) : SRMPatch(rPlacedPatch.size, rPlacedPatch.szFileName, rPlacedPatch.szPlace), minXYCorner(rPlacedPatch.minXYCorner), riversPoints(rPlacedPatch.riversPoints), roadsPoints(rPlacedPatch.roadsPoints) {}

  SRMPatch &operator=(const SRMPlacedPatch &rPlacedPatch)
  {
    if (&rPlacedPatch != this)
    {
      size = rPlacedPatch.size;
      szFileName = rPlacedPatch.szFileName;
      szPlace = rPlacedPatch.szPlace;
      minXYCorner = rPlacedPatch.minXYCorner;

      riversPoints = rPlacedPatch.riversPoints;
      roadsPoints = rPlacedPatch.roadsPoints;
    }
    return *this;
  }

  // all points are considered
  // to exclude points, they must be thrown out of the sheets
  static bool GetAndRemoveClosestVSOPoints(int nType,// road - river ( SRMGraphLink::TYPE )
                                           SRMPlacedPatch &rStartPlacedPatch,// initial patch
                                           SRMPlacedPatch &rEndPlacedPatch,// final patch
                                           const std::string &rVSODescFileName,// VSO name
                                           int nPointSelectionType,// selection of points see above EPointSelectionType
                                           SVSOPoint *pBeginVSOPoint,// starting point
                                           SVSOPoint *pEndVSOPoint);// end point
};

using CRMPlacedPatchesList = std::vector<SRMPlacedPatch>;


// to build empty spaces
class CRMFieldGraph
{
  struct SPatch
  {
    std::vector<bool> marckedVertices;// visited vertices
    std::vector<CVec2> boundingPolygon;// see below
    CTRect<int> boundingRect;// rectangle, needed for additional links
    std::vector<std::vector<int>> linesIndices;// indexes of lines from roads and rivers in the lines array

    // 1(minx, maxy) 2(maxx, maxy)
    // *--------------*
    // |    
    // |              
    // |              
    // |side 0 |size 2
    // |              
    // |              
    // |    
    // *--------------*
    // 0(minx, miny) 3(maxx, miny)
    //
    void FillBoundingPolygon(const CTRect<int> &rBoundingRect);
  };

  struct SLink
  {
    int nPatchIndex;
    int nSideIndex;
    int nLineIndex;
  };

  struct SLine
  {
    SLink begin;
    SLink end;
    int nID;
    std::list<CVec2> points;
  };

  struct SCaret
  {
    int nPatchIndex;
    int nSideIndex;
    int nLineIndex;
  };

  // input data
  std::vector<SPatch> patches;
  std::vector<SLine> lines;

  static int GetNextVetexIndex(int nVertexIndex) { return ((nVertexIndex >= 3) ? 0 : (nVertexIndex + 1)); }
  static int GetPreviousVetexIndex(int nVertexIndex) { return ((nVertexIndex <= 0) ? 3 : (nVertexIndex - 1)); }
  void AddBorderLines(const CTRect<int> &rBoundingRect);
  int IndexLines();
  void UnmarkAllVertices();
  bool FindUnmarkedVertex(int *pPatchIndex, int *pVertexIndex);
  bool GetPolygonAndMarkVertices(int nBeginPatchIndex, int nBeginVertexIndex, std::list<CVec2> *pPolygon, int nMaximumItations);

public:
  // output
  std::list<std::list<CVec2>> inclusivePolygons;
  std::list<std::list<CVec2>> exclusivePolygons;

  void AddPatch(const CTRect<int> &rBoundingRect);
  void AddLine(const std::list<CVec2> &rPoints, int nID);
  bool ConnectLineToPatch(int nPatchIndex, int nLineIndexLeft, bool bBegin);
  bool FindPolygons(const CTRect<int> &rBoundingRect);

  const CVec2 &GetPatchMinXYVertex(int nPatchIndex) { return patches[nPatchIndex].boundingPolygon[0]; }
  SPatch &GetPatch(int nPatchIndex) { return patches[nPatchIndex]; }
  SLine &GetLine(int nLineIndex) { return lines[nLineIndex]; }
  int GetLinesCount() { return lines.size(); }
};


using CRMUnitsPlaceHoldersHashMap = std::hash_map<DWORD, CWeightVector<std::string>>;
using CRMUnitsPlaceHoldersMnemonicsHashMap = std::hash_map<std::string, CWeightVector<std::string>>;


struct SRMTemplateUnitsTable
{
protected:
  static std::hash_map<DWORD, int> unitRPGTypeToIndex;
  static std::hash_map<std::string, int> unitRPGMnemonicToIndex;

public:
  struct SRandomMissionBonus
  {
    std::string szRPGStats;// RPG stats name
    int nWeight;// weight
    //
    int operator&(IDataTree &ss)
    {
      CTreeAccessor saver = &ss;
      saver.Add("RPGStats", &szRPGStats);
      saver.Add("Weight", &nWeight);
      return 0;
    }

    int operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &szRPGStats);
      saver.Add(2, &nWeight);
      return 0;
    }
  };

  //
  static const int UNIT_RPG_TYPE_COUNT;
  static const DWORD INVALID_UNIT_RPG_TYPE;
  static const DWORD SQUAD_UNIT_RPG_TYPE;
  static const DWORD UNIT_RPG_TYPES[];
  static const DWORD DEFAULT_LEVELS_COUNT;
  static const std::string UNIT_RPG_MNEMONICS[];

  static int UnitRPGTypeToIndex(DWORD nUnitRPGType);
  static int UnitRPGMnemonicToIndex(const std::string &rszUnitRPGMnenonic);

  static std::string GetUnitRPGMnemonic(DWORD nUnitRPGType);
  static DWORD GetUnitRPGType(const std::string &rszUnitRPGMnenonic);

  std::vector<CRMUnitsPlaceHoldersHashMap> unitPlaceHolders;// by unit type
  SUnitCreationInfo unitCreationInfo;// everything except appear points is written here
  std::vector<SRandomMissionBonus> bonuses;// random mission bonuses
  //
  const std::string GetRandomBonus() const;

  void GetRandomBonuses(std::vector<std::string> &_bonuses) const
  {
    _bonuses.clear();
    for (auto it = bonuses.begin(); it != bonuses.end(); ++it) { for (int nWeightIndex = 0; nWeightIndex < it->nWeight; ++nWeightIndex) { _bonuses.push_back(it->szRPGStats); } }
    std::random_shuffle(_bonuses.begin(), _bonuses.end());
  }

  void GetAllRandomBonuses(std::vector<std::string> &_bonuses) const
  {
    _bonuses.clear();
    _bonuses.reserve(bonuses.size());
    for (auto it = bonuses.begin(); it != bonuses.end(); ++it) { if (it->nWeight > 0) { _bonuses.push_back(it->szRPGStats); } }
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


struct SRMContext
{
  std::vector<SRMTemplateUnitsTable> levels;// by difficulty level
  //
  const std::string GetRandomBonus(const int nLevel) const
  {
    if (levels.empty()) { return ""; }
    if (nLevel >= levels.size()) { return levels[levels.size() - 1].GetRandomBonus(); }
    return levels[nLevel].GetRandomBonus();
  }

  //
  void GetRandomBonuses(const int nLevel, std::vector<std::string> &bonuses) const
  {
    if (levels.empty()) { bonuses.clear(); }
    else if (nLevel >= levels.size()) { levels[levels.size() - 1].GetRandomBonuses(bonuses); }
    else { levels[nLevel].GetRandomBonuses(bonuses); }
  }

  //
  void GetAllRandomBonuses(const int nLevel, std::vector<std::string> &bonuses) const
  {
    if (levels.empty()) { bonuses.clear(); }
    else if (nLevel >= levels.size()) { levels[levels.size() - 1].GetAllRandomBonuses(bonuses); }
    else { levels[nLevel].GetAllRandomBonuses(bonuses); }
  }

  // returns the number of objects and objects for the player
  int GetAvailiableUnits(int nPlayer, DWORD nUnitRPGType, std::hash_set<std::string> *pAvailableUnits)
  {
    for (int nLevelIndex = 0; nLevelIndex < levels.size(); ++nLevelIndex)
    {
      SRMTemplateUnitsTable &rTemplateUnitsTable = levels[nLevelIndex];
      if (nPlayer < rTemplateUnitsTable.unitPlaceHolders.size())
      {
        const CWeightVector<std::string> &rAvailiableUnits = rTemplateUnitsTable.unitPlaceHolders[nPlayer][nUnitRPGType];
        for (int nUnitIndex = 0; nUnitIndex < rAvailiableUnits.size(); ++nUnitIndex) { pAvailableUnits->insert(rAvailiableUnits[nUnitIndex]); }
      }
    }
    return pAvailableUnits->size();
  }

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
  bool IsValid(int nLevelsCount, int nPlayersCount);
};


struct SRMSetting
{
  CRMFieldSetWeightVector fields;// Information on filling empty spaces

  // serializing...
  virtual int STDCALL operator&(IStructureSaver &ss);
  virtual int STDCALL operator&(IDataTree &ss);
};


struct SRMUsedTemplateInfo
{
  std::string szTemplateName;
  std::string szGraphName;
  std::string szContextName;
  std::string szSettingName;
  int nGraphAngle;
};


#endif // __RMG_TYPES_H__