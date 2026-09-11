// TetraSurface.h: interface for the CTetraSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRASURFACE_H__2D78EFB8_A4B9_463F_AD6A_B46D7AA1E379__INCLUDED_)
#define AFX_TETRASURFACE_H__2D78EFB8_A4B9_463F_AD6A_B46D7AA1E379__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoSurface.h" 

class CTetraSurface : public CSurfaceBase
{
public:
  typedef CSingleCommandTemplate<CTetraSurface> TTetraSurfaceCommand;
protected:
  virtual CSurfaceBase* OnCreateSurface(const QString& sName, const geo::CPtrArray<geo::IPoint>& vcPoint);
  CSurfaceBase* OnCreateSurface(const QString& sName,const std::vector<const geo::IFace*>& vcFaces);
public:
  CTetraSurface(CFemAppModel& model);
  CTetraSurface(CSurfaceValueType &value_type);
  CTetraSurface(const QString &strName, const geo::ISurface& surface, CFemAppModel& model);
  CTetraSurface(const QString &strName, const std::vector<const geo::IFace*> &vcFace, CFemAppModel& model);
  CTetraSurface(const QString &strName, const geo::CPtrArray<geo::IPoint> &vcPoint, CFemAppModel& model);
  
  void CreateSubHorizon();
  void CreateSideSurface();
  void CreateSuperHorizon();
  void CreateSubFault();
  virtual const CGraphNode* Used() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTetraSurface);

private:
  const CGraphNode* UsedByModelOrChildModel(const CModelBase& model) const;
};

#endif // !defined(AFX_TETRASURFACE_H__2D78EFB8_A4B9_463F_AD6A_B46D7AA1E379__INCLUDED_)
