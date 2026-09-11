// NamedDrawDef.h: interface for the NamedDrawDef class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAMEDDRAWDEF_H__67FA68B2_09DA_43BF_8713_0840CACA769E__INCLUDED_)
#define AFX_NAMEDDRAWDEF_H__67FA68B2_09DA_43BF_8713_0840CACA769E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawDef.h"
#include "qcolor.h"

class NamedDrawDef : public CDrawDef {
  QString m_name;

public:
  QString Name() const;
  NamedDrawDef(const QString &name, const QRgb &color);
  NamedDrawDef(const QString &name);
};

#endif // !defined(AFX_NAMEDDRAWDEF_H__67FA68B2_09DA_43BF_8713_0840CACA769E__INCLUDED_)
