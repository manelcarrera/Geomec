// DexWellInterface.h: interface for the CDexWellInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEXWELLINTERFACE_H__59A19C8D_4088_41EE_AF63_C3D921CC633B__INCLUDED_)
#define AFX_DEXWELLINTERFACE_H__59A19C8D_4088_41EE_AF63_C3D921CC633B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "idexinterface.h"
#include <qstring.h>
#include <string>

class well::CWellPathBase;
class QString;

class AFX_EXT_CLASS CDexWellInterface : public IDexInterface {

  well::CWellPathBase &m_WellPath;
  QString m_app_name; // name of application writing the file
  QString m_version;  // version nr. (of whatever)

  enum WP_GEOMETRY_INPUT_TYPE { SURVEY = 0, PLAN };

  QVector<double> m_vc_inc;
  QVector<double> m_vc_azi;
  QVector<double> m_vc_tvd;
  QVector<double> m_vc_tmd;

public:
  CDexWellInterface(well::CWellPathBase &well_path, QString app_name, QString version);
  CDexWellInterface(well::CWellPathBase &well_path);
  virtual ~CDexWellInterface();

  BOOL ImportWellPathFromDEX(QString path_name, QString &error_msg);
  BOOL ImportFormationSections(QString path_name, QString &strError, QList<double> &tmd_values, QStringList &names);
  //	BOOL ImportCasingDefinition(QString path_name,QString &strError, QPtrList<SCasingDefinition> &casing_definitions);

  BOOL ExportToDEX(QString path_name, QString file_name, QString &error_msg) const;

  void AppName(QString app_name) { m_app_name = app_name; }
  void Version(QString version) { m_version = version; }
  QString BrowseForDEXFile();

private:
  // Interface functions
  const QString &AppName() const { return m_app_name; }
  const QString &Version() const { return m_version; }
  QString WellRepresentation() const;

  // Export functions
  void ExportGeneralWellInformation() const;
  void ExportDetailWellInformation() const;
  void ExportWellPlans() const;

  // Import functions
  void ImportGeneralWellInfo();
  void ImportDetailWellInfo();
  void ImportWellGeometry(WP_GEOMETRY_INPUT_TYPE geom_type);

  void SetWellPathGeometry();
  //	BOOL ImportCasingGrades();
};

#endif // !defined(AFX_DEXWELLINTERFACE_H__59A19C8D_4088_41EE_AF63_C3D921CC633B__INCLUDED_)
