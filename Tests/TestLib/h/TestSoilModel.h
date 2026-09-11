 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __SOIL_MODEL_H
#define __SOIL_MODEL_H

#include "DocumentBase.h"
#include <qtextstream.h>


namespace test_lib
{


class CMaterialContainer;
class CHorizonContainer;
class CFormationContainer;

class CSoilModel : public CDocumentBase
{
public:
  CSoilModel(const QString& sName, QTextStream &os);
  virtual ~CSoilModel();

  virtual CStreamVersion documentVersion() const;
  virtual QString documentType() const;

  CMaterialContainer& Materials() { return *m_pMaterials; }
  CHorizonContainer& Horizons() { return *m_pHorizons; } 
  CFormationContainer& Formations() { return *m_pFormations; } 

protected:
  QTextStream *m_os;
  virtual void createContainers();

private:
  CMaterialContainer* m_pMaterials;
  CHorizonContainer* m_pHorizons;
  CFormationContainer* m_pFormations;
};


}

#endif // __SOIL_MODEL_H
