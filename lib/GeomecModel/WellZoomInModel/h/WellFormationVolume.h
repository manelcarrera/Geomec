#ifndef _WELLZOOMINFORMATIONVOLUME_H_
#define _WELLZOOMINFORMATIONVOLUME_H_

#include "FormationVolume.h"
#include "GeomecModelVisitor.h"

class CWellFormation;

class CWellFormationVolume : public CFormationVolume {
public:
  CWellFormationVolume(CWellFormation &formation, geo::CBodyGroup &group);

  // Identifier
  virtual TGraphNodeSet Identifier() const;

  // Display lists (for skin mode)
  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellFormationVolume);
};

#endif // _WELLZOOMINFORMATIONVOLUME_H_
