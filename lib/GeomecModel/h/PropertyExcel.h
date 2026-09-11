// PropertyExcel.h: interface for the CPropertyExcel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYEXCEL_H__D737D5A0_DDB3_4803_8567_ADFD81F769C5__INCLUDED_)
#define AFX_PROPERTYEXCEL_H__D737D5A0_DDB3_4803_8567_ADFD81F769C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNodalExportFormat;
class IElementSetExportFormat;
class CNewWellPathInput;
class CElementSetExcelFileImpl;
class CModelBase;
class IProgressBase;

#include "GraphNode.h"

class CElementSetExcelFile
{
public:
// Contruction / Destruction
	CElementSetExcelFile
	( CModelBase& model
	, bool bNewWellPath = false
	);		// Constructor for reading
	CElementSetExcelFile(const CNodalExportFormat& export_format);	
	CElementSetExcelFile(const IElementSetExportFormat& export_format);
	~CElementSetExcelFile();
	CNewWellPathInput *NewWellPathInput() const;  // wjrx mantis 3322


	bool ReadSheets(const QString &sFileName, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);
	bool Write(const QString &sFileName, IProgressBase &callback, bool append = false );

//	typedef CElementSetFile::INodalFormat::TAxisSystem TAxisSystem;
	// Axis system
//	TAxisSystem AxisSystem() const;
	const CModelBase& Model() const;
	CModelBase& Model();

protected:
	// virtual pointset and elementset creation (for own types)

private:
  CElementSetExcelFile(const CElementSetExcelFile& rhs);
  CElementSetExcelFile& operator = (const CElementSetExcelFile& rhs);

  CElementSetExcelFileImpl* m_elementSetExcelFileImpl;
};

#endif // !defined(AFX_PROPERTYEXCEL_H__D737D5A0_DDB3_4803_8567_ADFD81F769C5__INCLUDED_)
