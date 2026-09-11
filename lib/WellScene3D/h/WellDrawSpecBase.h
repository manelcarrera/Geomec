// WellDrawSpecBase.h: interface for the CWellDrawSpecBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLDRAWSPECBASE_H__A34542F9_8690_4FB6_AEB3_D7D1C5BEA1D7__INCLUDED_)
#define AFX_WELLDRAWSPECBASE_H__A34542F9_8690_4FB6_AEB3_D7D1C5BEA1D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoObjectHandlerBase.h"
#include "LabelPoint.h"

class CWellSceneInterMed;

//##ModelId=3F7AA97E036E
class CWellDrawSpecBase : public CGeoObjectHandlerBase 
{
	Q_OBJECT

public:

	bool GetVisible() const;
	void SetVisible(bool visible );

	QRgb GetColor() const;
	void SetColor(QRgb color );

	int GetLineWidth() const;
	void SetLineWidth(int linewidth);

	int GetPointSize() const;
	void SetPointSize(int pointsize);

	QRgb GetTextColor() const;
	void SetTextColor(QRgb color);

	QRgb ArrowColor() const;
	void ArrowColor(QRgb color);

	QString GetDescription() const;
	void SetDescription(QString description );
	
	virtual ~CWellDrawSpecBase();

	CWellSceneInterMed& InterMed();

	CLabelPoint::eLabelAlign GetAlign() const;
	void SetAlign(CLabelPoint::eLabelAlign align) ;

	
protected:
	virtual void CreateScene()=0;

	CWellDrawSpecBase(CWellSceneInterMed& WellSceneInterMed);
	CWellSceneInterMed* m_pWellSceneInterMed;

	void DeleteAllDisplayList();
	void UpdateScene();

	void OnSceneDetached();
	void OnSceneAttached();


	int      m_PointSize;
	int      m_LineWidth;
	bool     m_Visible;
	QRgb m_Color;
	QRgb m_TextColor;
	QRgb m_ArrowColor;
	QString  m_Description;	
	CLabelPoint::eLabelAlign m_Align;
};

#endif // !defined(AFX_WELLDRAWSPECBASE_H__A34542F9_8690_4FB6_AEB3_D7D1C5BEA1D7__INCLUDED_)
