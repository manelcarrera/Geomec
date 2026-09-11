 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __SOIL_MODEL_FACTORY_H
#define __SOIL_MODEL_FACTORY_H

#include "IModelObjectFactory.h"
#include <qtextstream.h>


namespace test_lib
{


// TODO: fix this os versus inheritance stuff, so that we don't have to give out 0 ptr items when we have no stream


class CSoilModelFactory : public IModelObjectFactory
{
protected:
  mutable QTextStream *m_os;
public:
  CSoilModelFactory();

  void setOS(QTextStream &os) { m_os = &os; }

  virtual IModelObject* createObject( const char* className ) const;
};


}

#endif // __SOIL_MODEL_FACTORY_H
