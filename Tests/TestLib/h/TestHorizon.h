 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __HORIZON_H
#define __HORIZON_H

#include "ModelContainer.h"
#include <qtextstream.h>


namespace test_lib
{


class CHorizon : public IModelObject
{
  Q_OBJECT

public:
  CHorizon(QTextStream &os);
  CHorizon(const QString& sName, QTextStream &os);
  virtual ~CHorizon();

};

class CHorizonContainer : public CModelContainer<CHorizon>
{
  Q_OBJECT

public:
  CHorizonContainer(QTextStream &os);
  CHorizonContainer( const QString& text, QTextStream &os );
};


}

#endif // __HORIZON_H
