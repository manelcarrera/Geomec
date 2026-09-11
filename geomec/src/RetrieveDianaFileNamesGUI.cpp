#include "StdAfx.h"

#include "RetrieveDianaFileNamesGUI.h"
#include "DatFileDialog.h"
#include "FemAppMainWindow.h"

#include "QuestionThreadWrapper.h"
#include "Global.h"

CRetrieveDianaFileNamesGUI::CRetrieveDianaFileNamesGUI(
  CAnalysisType::TAnalysisType analysisType, bool writeInputFiles,
  bool writeQuadDat, const CModelBase& model)
: IRetrieveDianaFileNames()
, m_analysisType(analysisType)
, m_writeInputFiles(writeInputFiles)
, m_writeQuadDat(writeQuadDat)
, m_model(model)
{
}

CRetrieveDianaFileNamesGUI::~CRetrieveDianaFileNamesGUI()
{
}

bool CRetrieveDianaFileNamesGUI::retrieveDianaFileNames(
  QString& newTitle,
  const std::string& title) const
{
  newTitle = title.c_str();
  newTitle = stripExtension(newTitle, ".gm4");
  newTitle = stripExtension(newTitle, ".gm5");

  // total length of basename should be max 80 characters
  // we will leave some space for _Dxxx

  if (newTitle.length() > MAX_BASENAME_LENGTH)
  {
  newTitle = newTitle.left(MAX_BASENAME_LENGTH);
  }

  // ask for file locations if we are writing input files

  if (!m_writeQuadDat && !m_writeInputFiles)
  {
    newTitle = "";
    return true;
  }

  //
  // TODO: keep the way before for non-dsa ?
  //
  int res=IDCANCEL;
  std::string pathname;

  CDatFileDialog::Data* d = new CDatFileDialog::Data
  {
    m_analysisType, 
    m_writeInputFiles, 
    m_writeQuadDat,
    m_model, 
    0, // timeout
    //
    // CTnoFileDialog
    //
    FALSE, "",
    newTitle.toStdString().c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    "DIANA command/data files (*.dcf;*.dat)|*.dcf;*.dat|"
    "All Files (*.*)|*.*||", FemAppGetMainWnd()
  };

  if(_g->dsa())
  {
    CQuestionThreadWrapper th(CQuestionThreadWrapper::DatFileDlg,d); // event send to doc / wait for the resposne / blocking
    th.join();

    res = th.response();
    pathname = th.response_s();

    // data deleted at CQuestionThreadWrapper
  }
  else
  {
    CDatFileDialog dlgdat(d);

    res = dlgdat.DoModal();
    if (res == IDOK)
      pathname = (LPCSTR)dlgdat.GetPathName();
    delete d;
  }
  

  // dat-file

  if (res != IDOK)
    return false;

  newTitle = stripExtension( QString::fromStdString( pathname ), ".dcf"); //dlgdat.GetPathName()
  newTitle = stripExtension(newTitle, ".dat");

  return true;
}

// private

QString CRetrieveDianaFileNamesGUI::stripExtension(const QString& string,
  const QString& extension) const
{
  QString strippedString = string;
  int position = string.indexOf(extension);

  if (position > 0)
  {
  strippedString = string.left(position);
  }

  return strippedString;
}
