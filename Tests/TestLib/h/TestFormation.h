/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __FORMATION_H
#define __FORMATION_H

#include "ModelContainer.h"
#include <qtextstream.h>

namespace test_lib {

class CHorizon;
class CMaterial;
class CFormation : public IModelObject {
  Q_OBJECT

  CHorizon *m_pUpper;
  CHorizon *m_pLower;
  CMaterial *m_pMaterial;

protected:
  virtual void referenceRemoved(const IModelObject &origin, const IModelObject &reference);

public:
  CFormation(QTextStream &os);
  CFormation(const QString &sName, QTextStream &os);
  virtual ~CFormation();

  CHorizon *Upper();
  CHorizon *Lower();

  void Upper(CHorizon *pUpper);
  void Lower(CHorizon *pLower);

  CMaterial *Material();
  void Material(CMaterial *pMaterial);

  virtual void storeReferences(TStream &stream) const;
  virtual void restoreReferences(TStream &stream, const CStreamVersion &fileVersion);
};

class CFormationContainer : public CModelContainer<CFormation> {
  Q_OBJECT

public:
  CFormationContainer(QTextStream &os);
  CFormationContainer(const QString &text, QTextStream &os);
};

} // namespace test_lib

#endif // __FORMATION_H
