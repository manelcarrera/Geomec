// 3DFormation.h: interface for the C3DFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DFORMATION_H__E82D7010_1131_4D5E_838A_CC750FA5E72E__INCLUDED_)
#define AFX_3DFORMATION_H__E82D7010_1131_4D5E_838A_CC750FA5E72E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FormationBase.h"
#include "GeomecModelVisitor.h"

#include "BodyGroup.h"

#include "FormationVolume.h"

class CFormationVolume;
class C3DFormation : public CFormationBase
{
protected:
  C3DFormation(const C3DFormation& rhs);
  C3DFormation(CFemAppModel &model);
  C3DFormation(const QString &strName, CModelBase &model);
public:
  // Volume access
  virtual int VolumeSize() const; 
  virtual const CFormationVolume& Volume(int nIndex) const;
  virtual CFormationVolume& Volume(int nIndex);
  virtual bool Less(const CGraphNode &node) const;

  ACCEPT_GEOMECMODELVISITORS(Visit3DFormation);
};

class Casing3DFormation : public C3DFormation
{
  CFormationVolume m_volume;

public:
  Casing3DFormation(CFemAppModel &model, const geo::CBodyGroup& bodys);
  virtual const IFormationElementSet& ElementSet( int nIndex ) const;
  virtual int ElementSetSize() const;

  ACCEPT_GEOMECMODELVISITORS(Visit3DFormation);
};

#endif // !defined(AFX_3DFORMATION_H__E82D7010_1131_4D5E_838A_CC750FA5E72E__INCLUDED_)
