#ifndef _RESULTEXPORTER_H_
#define _RESULTEXPORTER_H_

#include <memory>

#include "ExportFormat.h"
#include "ExportSelection.h"
#include "HorizonBase.h"
#include "NewWellPath.h"
#include "Progress_.h" //CProgressCancel
#include "PropertyExcel.h"
#include "PropertyFile.h"
#include "WriteLogASCIIStandardFile.h"

#include "Global.h"

class IResultExporter {
public:
  IResultExporter(CModelBase &model);
  virtual ~IResultExporter();

  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const = 0;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const = 0;

  virtual void Export2LAS(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                          const IExportFormat::CExportArg arg) const;

  virtual const COpenGLNode *GetFirstNode() const = 0;

  CModelBase &Model() const;

protected:
  template <typename TExportFormat>
  bool exportExcel(TExportFormat &exportFormat, const QString &sFileName, bool bAppend) const;
  template <typename TExportFormat>
  bool exportASCII(TExportFormat &exportFormat, const QString &sFileName, bool bAppend) const;

private:
  CModelBase &m_model;
};

class CElementSetExporter : public IResultExporter {
public:
  CElementSetExporter(CModelBase &model, const std::vector<const IElementSet *> &vcElementset);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  std::vector<const IElementSet *> m_vcElementset;
};

class CFormationExporter : public IResultExporter {
public:
  CFormationExporter(CModelBase &model, const std::vector<const CFormationBase *> &vcFormation);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  CPointSet *CreateFormationCenterPointSet(const IExportFormat::TDataVec &vcData) const;

private:
  std::vector<const CFormationBase *> m_vcFormation;
};

class CFormationPlaneExporter : public IResultExporter {
public:
  CFormationPlaneExporter(CModelBase &model, const std::vector<const CFormationPlane *> &vcFormplane);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  CPointSet *CreateFormationPlaneCenterPointSet(const IExportFormat::TDataVec &vcData) const;
  static std::vector<const CFormationPlane *>
  sortFormationPlanes(const std::vector<const CFormationPlane *> &formationPlanes);

private:
  std::vector<const CFormationPlane *> m_vcFormplane;
};

class CHorizonExporter : public IResultExporter {
public:
  CHorizonExporter(CModelBase &model, const std::vector<const CHorizonBase *> &vcHorizon);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  CPointSet *CreateHorizonsPointSet(const IExportFormat::TDataVec &vcNodalData) const;

private:
  std::vector<const CHorizonBase *> m_vcHorizon;
};

class CSlipHorizonExporter : public IResultExporter {
public:
  CSlipHorizonExporter(CModelBase &model, const std::vector<const CHorizonBase *> &vcHorizon);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  std::vector<const CHorizonBase *> m_vcHorizon;
};

class CPointSetExporter : public IResultExporter {
public:
  CPointSetExporter(CModelBase &model, const std::vector<const CPointSet *> &vcPointset);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  CPointSet *CreateCombinedPointSet(const IExportFormat::TDataVec &vcNodalData) const;

private:
  std::vector<const CPointSet *> m_vcPointset;
};

class CRTCIExporter : public IResultExporter {
public:
  CRTCIExporter(CModelBase &model, const std::vector<const CWellCasingRTCI *> &vcRTCI);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  std::vector<const CWellCasingRTCI *> m_vcRTCI;
};

template <class WELLPATH> class CWellPathExporter : public IResultExporter {
public:
  CWellPathExporter(CModelBase &model, const std::vector<const WELLPATH *> &vcWellpath);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual void Export2LAS(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                          const IExportFormat::CExportArg arg) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  std::vector<const WELLPATH *> m_vcWellpath;
};

class CNonMeshedSurfaceExporter : public IResultExporter {
public:
  CNonMeshedSurfaceExporter(CModelBase &model, const std::vector<const CNonMeshedSurface *> &vcSurface);
  virtual void ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;
  virtual void ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                           const IExportFormat::CExportArg arg, bool bAppend = false) const;

  virtual const COpenGLNode *GetFirstNode() const;

private:
  std::vector<const CNonMeshedSurface *> m_vcSurface;
};

template <class WELLPATH>
CWellPathExporter<WELLPATH>::CWellPathExporter(CModelBase &model, const std::vector<const WELLPATH *> &vcWellpath)
    : IResultExporter(model), m_vcWellpath(vcWellpath) {}

template <class WELLPATH>
void CWellPathExporter<WELLPATH>::ExportExcel(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                                              const IExportFormat::CExportArg arg, bool bAppend) const {
  for (size_t i = 0; i < m_vcWellpath.size(); ++i) {
    CNodalExportFormat format(*m_vcWellpath[i], vcData, arg);
    CElementSetExcelFile file(format);
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, sFileName));
      file.Write(sFileName, *prog, bAppend);
    } catch (CProgressCancel *p) {
      delete p;
      return;
    }

    bAppend = true;
  }
}

template <class WELLPATH>
void CWellPathExporter<WELLPATH>::ExportASCII(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                                              const IExportFormat::CExportArg arg, bool bAppend) const {
  for (size_t i = 0; i < m_vcWellpath.size(); ++i) {
    CNodalExportFormat format(*m_vcWellpath[i], vcData, arg);
    CElementSetFile file(format);
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, sFileName));
      file.Save(sFileName, *prog, bAppend);
    } catch (CProgressCancel *p) {
      delete p;
      file.Close();
      return;
    }

    bAppend = true;
  }
}

template <class WELLPATH>
void CWellPathExporter<WELLPATH>::Export2LAS(const QString &sFileName, const IExportFormat::TDataVec &vcData,
                                             const IExportFormat::CExportArg arg) const {
  assert(m_vcWellpath.size() == 1);

  for (size_t i = 0; i < m_vcWellpath.size(); ++i) {
    CWriteLogASCIIStandardFile writeLogASCIIStandardFile(m_vcWellpath[i]);

    writeLogASCIIStandardFile.writeLogASCIIStandardFile(sFileName, vcData, arg);
  }
}

template <class WELLPATH> const COpenGLNode *CWellPathExporter<WELLPATH>::GetFirstNode() const {
  if (!m_vcWellpath.empty())
    return m_vcWellpath[0];

  return 0;
}

class CExporterFactory {
public:
  CExporterFactory(CModelBase &model);

  template <class T> const IResultExporter *CreateExporter(const CTypedSelection<T> &selection) {
    if (selection.empty())
      return 0;

    return CreateTypedExporter(MakeVector<T>(selection));
  }

private:
  template <class T> const IResultExporter *CreateTypedExporter(const std::vector<const T *> &vcSelection);
  template <class T> std::vector<const T *> MakeVector(const CTypedSelection<T> &selection);

private:
  CModelBase &m_model;
};

template <class T>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const T *> &vcSelection) {
  return 0;
}

// declare specializations
template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CHorizonBase *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const IElementSet *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CFormationBase *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CFormationPlane *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CPointSet *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CWellCasingRTCI *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CNewWellPath *> &vcSelection);

template <>
const IResultExporter *CExporterFactory::CreateTypedExporter(const std::vector<const CNonMeshedSurface *> &vcSelection);

template <class T> std::vector<const T *> CExporterFactory::MakeVector(const CTypedSelection<T> &selection) {
  std::vector<const T *> vcRet;
  for (typename CTypedSelection<T>::const_iterator it = selection.begin(); it != selection.end(); ++it) {
    vcRet.push_back(&*it);
  }

  return vcRet;
}

#endif // _RESULTEXPORTER_H_
