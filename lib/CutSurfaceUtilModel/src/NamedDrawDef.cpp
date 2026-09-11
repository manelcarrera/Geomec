// NamedDrawDef.cpp: implementation of the NamedDrawDef class.
//
//////////////////////////////////////////////////////////////////////



#include "NamedDrawDef.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NamedDrawDef::NamedDrawDef(const QString& name,const  QRgb&  color)
:CDrawDef(color),m_name(name)
{

}

//
NamedDrawDef::NamedDrawDef(const QString& name)
:CDrawDef( QColor(0,0,255).rgb() ) ,m_name(name)
{
}

QString NamedDrawDef::Name() const
{
  return m_name;
}
