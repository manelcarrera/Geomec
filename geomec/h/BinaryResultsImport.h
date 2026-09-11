#ifndef _BINARYRESULTSIMPORT_H_
#define _BINARYRESULTSIMPORT_H_

class CResultImportFormat;

class QFile;

class CBinaryResultsImport
{
public:
  CBinaryResultsImport(CModelBase& model);
  ~CBinaryResultsImport();

  bool Open(const CString& strPath, IProgressBase& progress);
  void Close();

  QString ErrorMessage() const;

private:
  void SetProgress(IProgressBase& progress, std::ifstream& s);
  QString GetLine(std::ifstream& s) const;

private:
  CModelBase& m_model;
  CResultImportFormat* m_pFormat;
  std::ifstream::pos_type m_endpos;
  int m_nProgress;
  QString m_strErrorMessage;
};

#endif // _BINARYRESULTSIMPORT_H_
