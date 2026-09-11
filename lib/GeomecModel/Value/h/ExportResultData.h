#ifndef _ExportResultData_h_
#define _ExportResultData_h_

#include "LocalResult.h"
#include "OpenGLNodeSelection.h"

class CDepletionStage;
class CHotSpot;

#include "colornode.h"
#include "GeomecModelVisitor.h"

#include "DepletionStage.h"
#include "FormationPlane.h"
#include "IColorScaleEntry.h"

class CExportResultData;


class IExportResultDataFilter
{
public:
  typedef enum { NONE = 0, SET = 1, BYTYPE = 2, BYNAME = 3 } TType;
};


class CExportResultDataSettings : public IExportResultDataFilter
{
public:

  QString m_folder;
  QString m_file;
  bool m_export;
  int m_nUnit;
  int m_nTZ;
  bool m_bLinear;
  bool m_bNonLinear;
  bool m_bHeat;
  bool m_bMixture;
  bool m_bMixtureContainment;
  int m_nCenterPoints;
  int m_nWellPoints;
  int m_nFormationNames;
  int m_nWeightingType;
  bool m_bReservoirMacro;

  CExportResultDataSettings();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData, bool bStore);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataDepletionStageFilter : public IExportResultDataFilter
{
public:

  TType m_nType;

  typedef enum { INITIAL = 0, LAST, ALL } TSetValue;

  union
  {
  TSetValue m_nSetValue;
  CDepletionStage::eOutputType m_nTypeValue;
  };

  QString   m_sName;

  CExportResultDataDepletionStageFilter();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataDepletionStageFilterVector : public IExportResultDataFilter
{
public:

  std::vector<CExportResultDataDepletionStageFilter> m_depletionStageFilters;

  CExportResultDataDepletionStageFilterVector();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData, bool bStore);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataModelFilter : public IExportResultDataFilter
{
public:

  typedef enum { FAULTS = 0, HORIZONS, FORMATIONS, FORMATIONPLANES, WELLPATHS, POINTSETS, ELEMENTSETS, NONMESHEDSURFACES } TFilter;

  TFilter m_nFilter;

  TType m_nType;

  typedef enum { ALL = 0 } TSetValue;

  TSetValue m_nSetValue;
  QString m_sName;

  typedef enum { PLANE_NONE = 0, PLANE_TOP, PLANE_MIDDLE, PLANE_BOTTOM } TPlaneMask;

  int m_nPlaneMask;

  CExportResultDataModelFilter();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataModelFilterVector : public IExportResultDataFilter
{
  typedef std::vector<CFormationPlane::ePlaneType> TFormationPlaneTypes;
  typedef std::map<const CHexaFormation *, TFormationPlaneTypes> TFormationPlaneTypeSet;

public:

  std::vector<CExportResultDataModelFilter> m_modelFilters;

  CExportResultDataModelFilterVector();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData, bool bStore);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataResultFilter : public IExportResultDataFilter
{
public:

  typedef enum { MATERIALS = 0, MESH, RESULTS, GVTRESULTS } TFilter;

  TFilter m_nFilter;

  TType m_nType;

  typedef enum { ALL = 0, NORMAL, FRACTUREINTENSITY } TSetValue;

  TSetValue m_nSetValue;
  QString m_sName;

  CExportResultDataResultFilter();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataResultFilterVector : public IExportResultDataFilter
{
public:

  IGeomecModelVisitor *m_matResults;
  IGeomecModelVisitor *m_meshResults;
  IGeomecModelVisitor *m_results;
  IGeomecModelVisitor *m_gvtResults;

  std::vector<IValueComponentBase *> *m_flatMatResults;
  std::vector<const IValueComponentBase *> *m_flatResults;
  std::vector<const IValueComponentBase *> *m_flatDerivedResults;
  std::vector<const IValueComponentBase *> *m_flatGVTResults;
  std::vector<const IValueComponentBase *> *m_flatGVTDerivedResults;

  std::vector<CExportResultDataResultFilter> m_resultFilters;

  CExportResultDataResultFilterVector();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData, bool bStore);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);

private:
  void unbindMaterialResults(CExportResultData& exportResultData);
  void unbindResults(CExportResultData& exportResultData);
  void unbindGVTResults(CExportResultData& exportResultData);
};


class CExportResultDataDeformation : public IExportResultDataFilter
{
public:

  TType m_nType;

  typedef enum { INITIAL = 0, LAST } TSetValue;

  TSetValue m_nSetValue;
  QString   m_sName;

  typedef enum { LINEAR = 0, NONLIN, MIXTURE } TCalculation;

  TCalculation m_nCalculation;


  CExportResultDataDeformation();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData, bool bStore);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};

class CExportResultDataHotSpot : public IExportResultDataFilter
{
public:

  TType m_nType;

  QString m_sName;

  bool m_bCenterIsTransparent;

  IColorScaleNode::TColor m_nMinColor;
  IColorScaleNode::TColor m_nMaxColor;

  std::pair<double, double> m_dMinMaxValue;

  CExportResultDataHotSpot();

  void bind(CExportResultData& exportResultData);
  void unbind(CExportResultData& exportResultData, bool bStore);

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
};





class CExportResultData : public CColorNode
{
public:
  CExportResultData(CFemAppModel& model);
  CExportResultData(const QString& exportResultName, CFemAppModel& model);

  // Identification.

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  // Rendering and default color.

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);


  CExportResultDataSettings m_settings;
  CExportResultDataDepletionStageFilterVector m_depletionStageFilterVector;
  CExportResultDataModelFilterVector m_modelFilterVector;
  CExportResultDataResultFilterVector m_resultFilterVector;
  CExportResultDataDeformation m_deformation;
  CExportResultDataHotSpot m_hotspot;

  void bind();
  void unbind(bool bStore);

  bool ReservoirMacro() const;


  typedef std::set <const CDepletionStage*> TTimeStepSet;

  QString m_folder;
  QString m_file;
  bool m_export;
  mutable COpenGLNodeSelection m_selection;
  int m_nUnit;
  int m_nTZ;
  const CHotSpot* m_pHotSpot;
  bool m_bLinear;
  bool m_bNonLinear;
  bool m_bHeat;
  bool m_bMixture;
  bool m_bMixtureContainment;
  TResultComponentSet m_RGObserverData;
  const CDepletionStage* m_pTimeStep;
  int m_nLinear;
  int m_nCenterPoints;
  int m_nWellPoints;
  int m_nFormationNames;
  int m_nWeightingType;
  TTimeStepSet m_stTimeStep;
  TResultComponentSet m_MatRGObserverData;
  TResultComponentSet m_GVTRGObserverData;

  bool m_bReservoirMacro;

  ACCEPT_GEOMECMODELVISITORS(VisitExportResultData);

private:
  CExportResultData(const CExportResultData& rhs);
  CExportResultData& operator = (const CExportResultData& rhs);
};

#endif  // _ExportResultData_h_
