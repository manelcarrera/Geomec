 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MATERIAL_H
#define __MATERIAL_H

#include "ModelContainer.h"
#include <qtextstream.h>


namespace test_lib
{


class CMaterial : public IModelObject
{
  Q_OBJECT

public:
  CMaterial(QTextStream &os);
  CMaterial(const QString& sName, QTextStream &os);
  virtual ~CMaterial();

};

class CMaterialContainer : public CModelContainer<CMaterial>
{
  Q_OBJECT

public:
  CMaterialContainer(QTextStream &os);
  CMaterialContainer( const QString& text, QTextStream &os );
};


}


#endif // __MATERIAL_H
