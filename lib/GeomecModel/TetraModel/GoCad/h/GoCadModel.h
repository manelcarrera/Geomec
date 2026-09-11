// GoCadModel.h: interface for the CGoCadModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADMODEL_H__7ADDE4A6_6905_4F28_97BF_3A1ECB075622__INCLUDED_)
#define AFX_GOCADMODEL_H__7ADDE4A6_6905_4F28_97BF_3A1ECB075622__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TetraModel.h"
class CHexaModel;
class CHexaHorizon;
class CHexaFormation;
class CTetraSubHorizon;
class CProgressDlg_MFC;
class CRotatedSystem;

namespace geo {
class ISurface;
}

#ifdef SKUA_NEW
namespace gm_skua {
struct SKUAParseData;
}
#endif

#include "GeomecModelVisitor.h"
#include "GocadData.h"

class CGoCadModel : public CTetraModel {
public:
  typedef std::map<double, std::set<double>> TLineMap;
  typedef IProgressBase TPROGRESS;

private:
  void ConvertSurfaces(const CHexaModel &model, TPROGRESS &progress);
  void ConvertCrossSections(const CHexaModel &model, TPROGRESS &progress);

  typedef std::map<const IPointSet *, const IPointSet *> TPointSetMap;
  void ConvertPointSets(const CHexaModel &model, TPROGRESS &progress, TPointSetMap &psMap);
  void ConvertHomoBoxes(const CHexaModel &model, TPROGRESS &progress);
  void ConvertDerivedResults(const CHexaModel &model, TPROGRESS &progress);
  void ConvertFormation(const CHexaFormation &formation, CTetraSubHorizon &upper, CTetraSubHorizon &lower);
  long OnCountSteps(const CHexaModel &model) const;
  void ConvertWellPaths(const CHexaModel &model, TPROGRESS &progress, TPointSetMap &psMap);
  CTetraSubHorizon *ConvertHorizon(const CHexaHorizon &horizon, TLineMap &Ymax, TLineMap &Ymin, TLineMap &Xmax,
                                   TLineMap &Xmin, TPROGRESS &progress);
  geo::ISurface *XSurface(const CRotatedSystem &rs, const TLineMap &Xmap, double XPos);
  geo::ISurface *YSurface(const CRotatedSystem &rs, const TLineMap &Ymap, double YPos);
  void AddPoints(const CHexaFormation &formation, TLineMap &line_map);
  void InsertMeshNodes();
  void UpdateProgress(CProgressDlg_MFC &dlg);

private:
  friend class IModelLifetimeFacade;

  CGoCadModel(CAnalysisLogger &logger, const CVersionManager &versionManager);
  virtual ~CGoCadModel();

public:
  virtual void createContainers();
  virtual bool Load(const QString &sPath, CProgressDlg_MFC &dlg);
  virtual bool Save(const QString &sPath, CProgressDlg_MFC &dlg);
  virtual void OnCloseModel();
  bool CanImportMesh() const;
#ifdef SKUA_NEW
  void ImportMesh(const QString &fileName,
                  bool (*selectGocadSolidDlg)(const std::vector<const gm_skua::SKUAParseData *> &solids,
                                              std::vector<int> &vcSolids));
#else
  void ImportMesh(const QString &fileName,
                  bool (*selectGocadSolidDlg)(const std::vector<QSharedPointer<CGocadData::CTSolid>> &solids,
                                              std::vector<int> &vcSolids));
#endif

  virtual ModelType modelType() const;

  void OnConvert(const CHexaModel &model, TPROGRESS &progress);

  CGoCadSubModelEntry *SubModelEntry();

  ACCEPT_GEOMECMODELVISITORS(VisitGoCadModel);
};

class CGoCadSubModelEntry : public TSubModelEntry {
public:
  typedef CSingleCommandTemplate<CGoCadSubModelEntry> TEntryCommand;

private:
  typedef CConstSingleCommandTemplate<CGoCadSubModelEntry> TConstEntryCommand;

public:
  CGoCadSubModelEntry(CTetraModel &model);

  bool CanInsertSurfaceContainer() const;
  // NB: A surface container is horizon or fault.

  virtual bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;

  ACCEPT_GEOMECMODELVISITORS(VisitGoCadSubModelEntry);
};

#endif // !defined(AFX_GOCADMODEL_H__7ADDE4A6_6905_4F28_97BF_3A1ECB075622__INCLUDED_)
