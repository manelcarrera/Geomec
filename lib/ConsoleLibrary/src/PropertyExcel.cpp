// PropertyExcel.cpp: implementation of the CPropertyExcel class.
//
//////////////////////////////////////////////////////////////////////

#include "PropertyExcel.h"
#include "ExportFormat.h"

class CElementSetExcelFileImpl {
public:
  CElementSetExcelFileImpl(CModelBase &model, bool newWellPath = false);
  CElementSetExcelFileImpl(const CNodalExportFormat &export_format);
  CElementSetExcelFileImpl(const IElementSetExportFormat &export_format);
  ~CElementSetExcelFileImpl();

private:
  CElementSetExcelFileImpl(const CElementSetExcelFileImpl &rhs);
  CElementSetExcelFileImpl &operator=(const CElementSetExcelFileImpl &rhs);

  const CNodalExportFormat *m_pNodalExportFormat;
  const IElementSetExportFormat *m_pElementSetExportFormat;
  CModelBase &m_model;
  bool m_bNewWellPath;
  CNewWellPathInput *m_pNewWellPathInput;

public:
  CNewWellPathInput *NewWellPathInput() const { return m_pNewWellPathInput; }

  bool ReadSheets(const QString &sFileName, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);
  bool Write(const QString &sFileName, IProgressBase &callback, bool append = false);

  const CModelBase &Model() const;
  CModelBase &Model();
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CElementSetExcelFileImpl::CElementSetExcelFileImpl(CModelBase &model, bool bNewWellPath)
    : m_pNodalExportFormat(0), m_pElementSetExportFormat(0), m_model(model), m_bNewWellPath(bNewWellPath),
      m_pNewWellPathInput(0) {}

CElementSetExcelFileImpl::CElementSetExcelFileImpl(const IElementSetExportFormat &format)
    : m_pNodalExportFormat(&format.NodalExportFormat()), m_pElementSetExportFormat(&format),
      m_model(const_cast<CModelBase &>(format.Model())), m_bNewWellPath(false), m_pNewWellPathInput(0) {}

CElementSetExcelFileImpl::CElementSetExcelFileImpl(const CNodalExportFormat &format)
    : m_pNodalExportFormat(&format), m_pElementSetExportFormat(0), m_model(const_cast<CModelBase &>(format.Model())),
      m_bNewWellPath(false), m_pNewWellPathInput(0) {}

CElementSetExcelFileImpl::~CElementSetExcelFileImpl() {}

bool CElementSetExcelFileImpl::ReadSheets(const QString & /*sFileName*/, IProgressBase & /*dlg*/,
                                          CGraphNode::TNodeVec & /*created_objects*/) {
  return false;
}

bool CElementSetExcelFileImpl::Write(const QString & /*sFileName*/, IProgressBase & /*callback*/, bool /*append*/) {
  return false;
}

const CModelBase &CElementSetExcelFileImpl::Model() const { return m_model; }

CModelBase &CElementSetExcelFileImpl::Model() { return m_model; }

// CElementSetExcelFile

CElementSetExcelFile::CElementSetExcelFile(CModelBase &model, bool bNewWellPath)
    : m_elementSetExcelFileImpl(new CElementSetExcelFileImpl(model, bNewWellPath)) {}

CElementSetExcelFile::CElementSetExcelFile(const CNodalExportFormat &export_format)
    : m_elementSetExcelFileImpl(new CElementSetExcelFileImpl(export_format)) {}

CElementSetExcelFile::CElementSetExcelFile(const IElementSetExportFormat &export_format)
    : m_elementSetExcelFileImpl(new CElementSetExcelFileImpl(export_format)) {}

CElementSetExcelFile::~CElementSetExcelFile() { delete m_elementSetExcelFileImpl; }

CNewWellPathInput *CElementSetExcelFile::NewWellPathInput() const {
  return m_elementSetExcelFileImpl->NewWellPathInput();
}

bool CElementSetExcelFile::ReadSheets(const QString &sFileName, IProgressBase &dlg,
                                      CGraphNode::TNodeVec &created_objects) {
  return m_elementSetExcelFileImpl->ReadSheets(sFileName, dlg, created_objects);
}

bool CElementSetExcelFile::Write(const QString &sFileName, IProgressBase &callback, bool append) {
  return m_elementSetExcelFileImpl->Write(sFileName.toStdString().c_str(), callback, append);
}

const CModelBase &CElementSetExcelFile::Model() const { return m_elementSetExcelFileImpl->Model(); }

CModelBase &CElementSetExcelFile::Model() { return m_elementSetExcelFileImpl->Model(); }
