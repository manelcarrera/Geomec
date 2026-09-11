// EclipseModel.h: interface for the CEclipseModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECLIPSEMODEL_H__EE973B72_FB02_4815_8C9E_DC69AD8700D6__INCLUDED_)
#define AFX_ECLIPSEMODEL_H__EE973B72_FB02_4815_8C9E_DC69AD8700D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TetraModel.h"

class CTetraSubHorizon;
class CTetraSurface;
class CEclipseReservoir;
class CEclipseFile;

class CEclipseModel : public CTetraModel {
  typedef std::vector<geo::CPoint> TRow;
  typedef std::vector<TRow> TEclipseHorizon;
  typedef std::pair<TEclipseHorizon, CTetraSubHorizon *> THorizon;
  typedef std::pair<int, int> TEclipseReservoir;

  std::vector<THorizon> m_vcHorizon;
  std::vector<TEclipseReservoir> m_vcReservoir;
  std::vector<CEclipseReservoir *> m_vcOriginalData; // if we ever need it.....

  int m_nUpperHorizon; // indices to the upper and lower horizon selected by the user
  int m_nLowerHorizon;
  CTetraSurface *m_pFrontSurface;
  CTetraSurface *m_pBackSurface;
  CTetraSurface *m_pLeftSurface;
  CTetraSurface *m_pRightSurface;
  QString m_fileName;

  void ExtractCells(
      const CEclipseReservoir *pReservoir); // extracts the cells from a reservoir and builds the m_mpCellPoints map.
  //	void MergePoints(); // Merges all the points of the cells in the m_vcCell, making up the complete model.
public:
  void InitFile(const CEclipseFile &file);
  //	std::vector<geo::CPoint> AdjacentPoints(const CEclipseFile& file, int Xp, int Yp, int Zp) const;
  //	void AddPoint(const CEclipseFile& file, std::vector<geo::CPoint>& vcPoint, int Xv, int Yv, int Zv, int px, int py,
  // int pz) const; 	geo::CPoint MeanPoint(const CEclipseFile& file, int Xp, int Yp, int Zp) const;
  void CreateSubBoundary();

private:
  CTetraSurface *Surface(int nIndex);

  friend class IModelLifetimeFacade;

  // Construction ...
  CEclipseModel(CAnalysisLogger &logger, const CVersionManager &versionManager);
  virtual ~CEclipseModel();

public:
  // Model creation : I  When new model is called the user must select a file and select horizons.
  //					II When build model from file is used the user must only select horizons
  virtual bool NewModel(bool bAttachToDocument = true);

  // Access to the orginal eclipse data read from the file
  int ReservoirSize() const;
  std::pair<int, int> Reservoir(int nReservoirIndex) const;
  int HorizonSize() const;
  const CTetraSubHorizon *Horizon(int nHorizonIndex) const;
  CTetraSubHorizon *Horizon(int nHorizonIndex);
  void HorizonEnable(int nHorizonIndex, bool bHorizonEnable);
  const geo::IPoint &HorizonPoint(int nHorizonIndex, int Xp, int Yp) const;
  int PointSizeX() const;
  int PointSizeY() const;

  virtual void createContainers();
  virtual void OnCloseModel();
  virtual long SavedItems() const;
  virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  bool LoadEclipse(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog);
  virtual unsigned int ModelIconId() const;
  virtual QString documentType() const;

  static unsigned int staticModelIconId();
  static QString staticDocumentType();

  virtual ModelType modelType() const;
  void setFileName(const QString &fileName);

  ACCEPT_GEOMECMODELVISITORS(VisitEclipseModel);
};

class CEclipseSubModelEntry : public TSubModelEntry {
public:
  typedef CSingleCommandTemplate<CEclipseSubModelEntry> TEntryCommand;

private:
  typedef CConstSingleCommandTemplate<CEclipseSubModelEntry> TConstEntryCommand;

public:
  CEclipseSubModelEntry(CEclipseModel &model);

  ACCEPT_GEOMECMODELVISITORS(VisitEclipseSubModelEntry);
};

#endif // !defined(AFX_ECLIPSEMODEL_H__EE973B72_FB02_4815_8C9E_DC69AD8700D6__INCLUDED_)
