// ModelGeneralPg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "ModelGeneralPg.h"
#include "ModelBase.h"
#include <sys/stat.h>

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelGeneralPg dialog


//##ModelId=3BC55D610278
CModelGeneralPg::CModelGeneralPg(CGeomecDoc &doc)
: CPropertyPage(CModelGeneralPg::IDD), m_doc(doc), m_stPath(DT_LEFT | DT_NOPREFIX | DT_VCENTER, TRUE)
{
  //{{AFX_DATA_INIT(CModelGeneralPg)

  //}}AFX_DATA_INIT
}


//##ModelId=3BC55D610279
void CModelGeneralPg::DoDataExchange(CDataExchange* pDX)
{
  CString strProjectInfo, strTitle, strPath, strModelType, strSize, strFileVersion, strAppVersion;

  CDialog::DoDataExchange(pDX);
  
  if(!pDX->m_bSaveAndValidate)
  {
    assert(m_doc.Model());
    strProjectInfo = ((CModelBase*)m_doc.Model())->ProjectInfo().toStdString().c_str();
    strTitle = m_doc.GetTitle();
    strPath = m_doc.GetPathName();
    strModelType = m_doc.Model()->documentType().toStdString().c_str();
  
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if( GetFileAttributesEx(strPath, GetFileExInfoStandard, &Data ) != -1 )
    {
      if( Data.nFileSizeLow != -1)
      {
        if((Data.nFileSizeLow / 1000000000.0) > 1.0)
          strSize.Format(_T("%6.3f Gb"), (Data.nFileSizeLow / 1000000000.0));
        else if((Data.nFileSizeLow / 1000000.0) > 1.0)
          strSize.Format(_T("%6.3f Mb"), (Data.nFileSizeLow / 1000000.0));
        else if((Data.nFileSizeLow / 1000.0) > 1.0)
          strSize.Format(_T("%6.3f kb"), (Data.nFileSizeLow / 1000.0));
        else strSize.Format(_T("%d byte"), Data.nFileSizeLow);
      }
      else
      {
        strSize = _T("0 kb");
      }
    }
    else{
      strSize = _T("0 kb");
    }

    // File version 
    CModelBase& modelBase = dynamic_cast <CModelBase&> (*m_doc.Model());
    strFileVersion.Format("%d.%d.%d", modelBase.documentVersion().majorNr(), modelBase.documentVersion().minorNr(), modelBase.documentVersion().revisionNr());
    strAppVersion = modelBase.getVersionManager().AppVersion().toStdString().c_str();
  }
  //{{AFX_DATA_MAP(CModelGeneralPg)
  DDX_Control(pDX, IDC_ST_PATH, m_stPath);
  //}}AFX_DATA_MAP

  if(!pDX->m_bSaveAndValidate)
    m_stPath.SetWindowText(strPath);

  DDX_Text(pDX, IDC_PROJECT_INFO, strProjectInfo);
  DDX_Text(pDX, IDC_ST_FILE_NAME, strTitle);
  DDX_Text(pDX, IDC_ST_FILE_SIZE, strSize);
  DDX_Text(pDX, IDC_ST_MODEL_TYPE, strModelType);
  DDX_Text(pDX, IDC_ST_FILE_VERSION, strFileVersion);
  DDX_Text(pDX, IDC_ST_APP_VERSION, strAppVersion);
  
  if(pDX->m_bSaveAndValidate)
  {
    assert(m_doc.Model());
    ((CModelBase*)(m_doc.Model()))->ProjectInfo((LPCSTR) strProjectInfo);
  }

}


BEGIN_MESSAGE_MAP(CModelGeneralPg, CDialog)
  //{{AFX_MSG_MAP(CModelGeneralPg)
  ON_EN_CHANGE(IDC_PROJECT_INFO, OnChangeProjectInfo)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelGeneralPg message handlers

void CModelGeneralPg::OnChangeProjectInfo() 
{
  SetModified(true);
}

