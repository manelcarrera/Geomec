// DCasing.h: interface for the CDCasing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASING_H__B02F6833_CB66_4BBD_BE41_3B2C9FE7094D__INCLUDED_)
#define AFX_DCASING_H__B02F6833_CB66_4BBD_BE41_3B2C9FE7094D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h>
#include <QVector>
#include <qfile.h>
#include <QTextStream>
#include "DCasingDefines.h"
#include "DCasinggeneralParameters.h"
#include "DCasingPoint.h"
#include "DCasintSlotHandler.h"

class CDCasing :public QObject 
{
	Q_OBJECT;

public:

//signals from diana process
signals:
	void OnMessage(QString str);
	void OnError(QString str);
	void OnWarning(QString str);
	void OnFinished(); 

public slots:
	void AbortCalculation();

public:

	void HandleMessage(QString str);
	void HandleError(QString str);
	void HandleWarning(QString str);
	void HandleFinished(); 

private:
	
	//member attributes
	//definition of the casing points
	typedef QVector<CDCasingPoint*> DCasingPoints;
	DCasingPoints m_points;
	unsigned long m_Version;
	unsigned long m_Revision;
	CDCasingGeneralParameters m_DCasingGeneralParameters;
public:
	// construction / destruction
	CDCasing();
	virtual ~CDCasing();

	//interface
	CDCasingPoint& AddPoint(CDCasingPoint & point);
	CDCasingPoint& Point(int index);
	
	void RemovePoint(int index);
	void RemoveAll();
	unsigned long NumberfPoints() const;

	void StartAnalysis();
	
	void WriteInputFile(const QString & FileName);
	void ReadOutputFile(const QString & FileName);
	
	CDCasingGeneralParameters & GetGeneralParameters();


private :

	//private member methods 
	void ReadSupportedValues(QFile & varFile);
	void ReadUnsupportedValues(QFile & varFile);
};

#endif // !defined(AFX_DCASING_H__B02F6833_CB66_4BBD_BE41_3B2C9FE7094D__INCLUDED_)
