#include "StdAfx.h"

#include "DatFileDialog.h"
#include "DepletionStage.h"
#include "DianaRunController.h"
#include "GlobalMessage.h"
#include "ModelBase.h"

static const UINT ID_MY_TIMER = 1000;

CDatFileDialog::CDatFileDialog(const Data *d)
    : CDatFileDialog(
          //
          // CDatFileDialog
          //
          d->analysisType, d->writeInputFiles, d->writeQuadDat, d->model, d->timeout,
          //
          // CTnoFileDialog
          //
          d->bOpenFileDialog, d->lpszDefExt, d->lpszFileName, d->dwFlags, d->lpszFilter, d->pParentWnd) {}

CDatFileDialog::CDatFileDialog(CAnalysisType::TAnalysisType analysisType, bool writeInputFiles, bool writeQuadDat,
                               const CModelBase &model, int timeout,
                               //
                               BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
                               LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter,
                               CWnd *pParentWnd)
    : CTnoFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd),
      m_analysisType(analysisType), m_writeInputFiles(writeInputFiles), m_writeQuadDat(writeQuadDat), m_model(model),
      m_timeout(timeout) {}

BEGIN_MESSAGE_MAP(CDatFileDialog, CTnoFileDialog)
//{{AFX_MSG_MAP(CDatFileDialog)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDatFileDialog::OnInitDialog() {
  CTnoFileDialog::OnInitDialog();

  if (m_timeout)
    SetTimer(ID_MY_TIMER, m_timeout, NULL);

  return TRUE;
}

//
// it works! timer is called
//
// but it fails, dlg is not ended
//
void CDatFileDialog::OnTimer(UINT_PTR nIDEvent) {
  if (nIDEvent == ID_MY_TIMER) {
    KillTimer(ID_MY_TIMER); // only once
    //
    // it doesn't work... why?
    //
    // this->m_ofn.lpstrFile = _T("my_file_name");
    // EndDialog(IDOK);						// -> it doesn't work
    // CTnoFileDialog::EndDialog(IDCANCEL);	// -> it doesn't work
    CFileDialog::EndDialog(IDCANCEL); // -> it doesn't work
    // OnCancel();							// -> it doesn't work
  }
}

//
// mcr 2020-10-05: nobody uses it so better to be commented to avoid confusion
//
#ifdef KK
BOOL CDatFileDialog::OnFileNameOK() {
  if (GetFileName().GetLength() - 4 > MAX_BASENAME_LENGTH) {
    QString strMsg;
    strMsg = QString("The basename (file name without extension including possibly _Dxxx) can not be longer than %1 "
                     "characters.\nCurrently the total length is %2 characters")
                 .arg(MAX_BASENAME_LENGTH + 5)
                 .arg(GetFileName().GetLength() + 1);
    _m()->msg(strMsg);
    return 1; // don't allow the dialog to be dismissed
  }

  if (m_analysisType != CAnalysisType::AT_NONLIN || (!m_writeInputFiles && !m_writeQuadDat))
    return CTnoFileDialog::OnFileNameOK(); // default behaviour

  const CDepletionStage *pStage = fileNames2GenerateExist(".dat");

  if (!pStage) {
    pStage = fileNames2GenerateExist(".dcf");
  }

  if (pStage) {
    QString strFileName = GetFileName();
    std::string sModFileName = pStage->ModifiedFileName(strFileName.toStdString());
    QString strMsg;
    strMsg = QString("At least one existing file (encountered '%1') will be replaced during the analysis\n\nDo you "
                     "want to continue?")
                 .arg(sModFileName.c_str());
    if (_m()->msg(strMsg, MB_YESNO) == IDNO)
      return 1; // don't allow the dialog to be dismissed
  }

  return 0;
}

// private

const CDepletionStage *CDatFileDialog::fileNames2GenerateExist(const QString &extension) const {
  QString strPath = GetPathName();

  strPath = stripExtension(strPath, ".dat");
  strPath = stripExtension(strPath, ".dcf");
  strPath += extension;

  // check the file names the analysis will generate
  const CDepletionStage *pStage = &m_model.InitialDepletionStage();
  while (pStage) {
    // generate the filename
    std::string sModFileName = pStage->ModifiedFileName(strPath.toStdString());

    // see if it exists
    struct _stat buf;
    if (_stat(sModFileName.c_str(), &buf) == 0) {
      break;
    }

    pStage = &pStage->GetNextBranchStage(*pStage);
    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  return pStage;
}

QString CDatFileDialog::stripExtension(const QString &string, const QString &extension) const {
  QString strippedString = string;
  int position = string.indexOf(extension);

  if (position > 0) {
    strippedString = string.left(position);
  }

  return strippedString;
}
#endif
