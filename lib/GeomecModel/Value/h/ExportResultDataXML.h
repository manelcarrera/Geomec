#ifndef _EXPORT_RESULT_DATA_XML_H_
#define _EXPORT_RESULT_DATA_XML_H_

#include <QString>
#include <vector>

class QDomElement;
class CExportResultData;
class CModelBase;

namespace erd_xml {

class CException {
public:
  CException(const QString &strError);
  const QString &error() const;

private:
  QString m_strError;
};

class CExportResultDataXML {
public:
  typedef std::vector<CExportResultData *> TExportResultDataVector;

  CExportResultDataXML(CModelBase &model, TExportResultDataVector &vcExportResultData);

  // these throw a CException on error
  void LoadFile(const QString &sFileName);
  void SaveFile(const QString &sFileName);

  // these throw a CException on error
  void Load(QDomElement &domElement);
  void Save(QDomElement &domElement);

private:
  CModelBase &m_model;
  TExportResultDataVector &m_vcExportResultData;
};

} // namespace erd_xml

#endif