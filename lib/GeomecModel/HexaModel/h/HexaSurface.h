// HexaSurface.h: interface for the CHexaSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXASURFACE_H__B8DA1155_BD3A_4761_8343_48CBF725D384__INCLUDED_)
#define AFX_HEXASURFACE_H__B8DA1155_BD3A_4761_8343_48CBF725D384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoSurface.h"

class CHexaSurface : public CSurfaceBase {
public:
  typedef CSingleCommandTemplate<CHexaSurface> THexaSurfaceCommand;

protected:
  virtual CSurfaceBase *OnCreateSurface(const QString &sName, const geo::CPtrArray<geo::IPoint> &vcPoint);

public:
  // Construction
  CHexaSurface(CFemAppModel &model);
  CHexaSurface(CSurfaceValueType &value_type);
  CHexaSurface(const QString &strName, const geo::ISurface &surface, CFemAppModel &model);
  CHexaSurface(const QString &strName, const geo::CPtrArray<geo::IPoint> &vcPoint, CFemAppModel &model);
  CHexaSurface(const QString &strName, const geo::CArray<geo::CPoint> &vcPoint, CFemAppModel &model);

  // Context menu for hexa surfaces ...
  void CreateHorizon(); // Just call the copy ...

  ACCEPT_GEOMECMODELVISITORS(VisitHexaSurface);
};

#endif // !defined(AFX_HEXASURFACE_H__B8DA1155_BD3A_4761_8343_48CBF725D384__INCLUDED_)
