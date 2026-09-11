#include "stdafx.h"

#include "GeomecDoc.h"
#include "GeomecHelper.h"

#include "ChangelogDlg.h"
#include "Item.h"
#include "Plugins.h"
#include "Printer.h"
#include "QUtil.h"
#include "Registry.h"
#include "Xml.h"

#include "version.h" //GM_BUILD_STRING

#include <QTimer>

namespace {
Printer *printer = Printer::instance(Printer::App);
}

namespace gm {
namespace helper {
namespace version {
// static const std::string KEY = "version";
// static const std::string CONFIG_FILE = std::string( getenv("APPDATA") ) + "/" + "Geomec" + "/" + "selector.xml";

// selector.xml
void unset() { CXml::update(CONFIG_FILE, version::KEY, ""); }

// from selector.xml
QString read() {
  std::string res = CXml::get(CONFIG_FILE, version::KEY);
  return res == CXml::NOT_FOUND ? "" : QString::fromStdString(res);
}

// to selector.xml
void set(const QString &version) { CXml::update(CONFIG_FILE, version::KEY, version.toStdString()); }

} // namespace version

ChangelogDlg *changelog(std::string release) {
  ChangelogDlg *dlg = nullptr;

  // is new release ?
  std::string build = QString::number(GM_BUILD_STRING).toStdString();

  printer->info("release: '%s' | build: '%s'", release.c_str(), build.c_str());

  if (QUtil::exists(ChangelogDlg::GEOMEC_CONFIG_FILE, QUtil::File)) {
  } else {
    bool res = QFile::copy(QString::fromStdString(CPlugins::LOCAL_PLUGINS_REPOSITORY + "//" + "geomec.xml"),
                           QString::fromStdString(ChangelogDlg::GEOMEC_CONFIG_FILE));

    printer->info("create : %s", ChangelogDlg::GEOMEC_CONFIG_FILE.c_str());

    if (!res)
      printer->error("create : %s", ChangelogDlg::GEOMEC_CONFIG_FILE.c_str());
  }

  CRegistry reg(release, build);
  CRegistry::eRelease res_c = reg.check();
  switch (res_c) {
  case CRegistry::eRelease::Same:
    break;
  case CRegistry::eRelease::NewRelease:
  case CRegistry::eRelease::NewBuild: {
    CXml::update(ChangelogDlg::GEOMEC_CONFIG_FILE, "show", "yes");
    printer->info("modify : %s : %s -> %s", ChangelogDlg::GEOMEC_CONFIG_FILE.c_str(), "show", "yes");

    // if( res_c == CRegistry::eRelease::NewRelease )
    //	bool res_i = InstallChangelog();
    break;
  }
  default:
    break;
  }
  reg.modify(CRegistry::BUILD_KEY, build);

  // show changelog dlg if needed
  CXml gm_cfg(ChangelogDlg::GEOMEC_CONFIG_FILE, {"show"});
  if (gm_cfg.verify()) {
    if (gm_cfg.in("show", {"yes", "y"}, CXml::eKeySensitive::No)) {
      dlg = new ChangelogDlg();
      if (dlg->verify())
        QTimer::singleShot(5 * 1000, dlg, SLOT(exec()));
      else {
        AfxMessageBox("No changelog data found", MB_ICONERROR);
        printer->info("No changelog data found");
      }
    }
  }
  return dlg;
}

void plugins() {
  // (1) Plugins to install
  CPlugins::instance()->install_repository_plugins();

  // (2) Menu: 'Plugins' -> add available plugins
  CPlugins::instance()->init();
  CItem::refresh(CItem::TPathV{CPlugins::DEF_PLUGINS_SUB_MENU_NAME}, CPlugins::instance()->get(), CItem::Launch);

  // (3) Menu: 'Plugins > Remove' -> add available plugins to remove menu
  CItem::TPathV remove_path_v = CItem::TPathV{"Plugins", "Remove"};
  CItem::refresh(remove_path_v, CPlugins::instance()->get(), CItem::Remove);

  // (4) Manual
  CItem::refresh({"&Help"}, CPlugins::instance()->get(),
                 CItem::Manual); // FIXME: '&Help' -> Make it independent from special characters
}

void add_default_version_menu_item() {
  QString v = gm::doc_::version();

  std::string caption = v == gm::helper::version::read() ? "Mark as NOT default version" : "Mark as default version";

  CItem::add({"&Settings"}, {caption}, WM_USER + 1000);
}

void qdebug_msg_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  std::string url = printer->folder() + "\\" + "geomec_qdebug.log";
  QFile qdebug_out_file(url.c_str());

  QString txt;

  // QString s_time_ = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
  QString s_time = QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss");

  switch (type) {
  // (%s:%u, %s)
  // context.file, context.line, context.function
  case QtDebugMsg:
    txt = QString("Debug: %1: %2").arg(s_time, msg);
    break;
  case QtWarningMsg:
    txt = QString("Warning: %1").arg(msg);
    break;
  case QtCriticalMsg:
    txt = QString("Critical: %1").arg(msg);
    break;
  case QtFatalMsg:
    txt = QString("Fatal: %1").arg(msg);
    break;
  }

  qdebug_out_file.open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream ts(&qdebug_out_file);
  ts << txt << endl;
  qdebug_out_file.close(); // ????
}

} // namespace helper
} // namespace gm

/*bool InstallChangelog()
{
  bool retval = false;

  if( QUtil::file_exists( ChangelogDlg::CHANGELOG_PACKAGE ) )
  {
    printer->info( "changelog : install : %s", ChangelogDlg::CHANGELOG_PACKAGE.c_str() );

    QString dir_qs = GetGeomecTempPathExt( CTempPath::TEMP_GENERAL );

    printer->info( "tmp dir : '%s'", dir_qs.toStdString().c_str() );

    if( QFileInfo( dir_qs ).isDir() )
    {
      //QString tmp_dir_path = QDir::toNativeSeparators(CreateTempDirExt(CTempPath::TEMP_GENERAL) + "/");
      QString tmp_dir_path = QDir::toNativeSeparators(CreateTempDirExt(CTempPath::TEMP_GENERAL) );
      std::string tmp_dir_path_s = tmp_dir_path.toStdString();

      if( QFileInfo( tmp_dir_path ).isDir() )
      {
        bool res_u  = CSystem::unpack(ChangelogDlg::CHANGELOG_PACKAGE, tmp_dir_path_s );
        if( res_u )
        {
          QUtil::TFilesV changelog_v = QUtil::files( tmp_dir_path_s, "*.zip", QUtil::eFile::FullPath );

          bool res;
          for( int i=0; i < changelog_v.size(); i++ )
          {
            res = CSystem::unpack( changelog_v[ i ], QUtil::usr_dir( QUtil::eUsrDir::Changelog ) );
            if( !res )
              break;
          }
          if( res )
          {
            //res = CPlugins::copy( tmp_dir_path_s + "/changelog.css", CPlugins::usr_dir( CPlugins::eUsrDir::Changelog )
+ "/changelog.css" ); res  = CSystem::unpack(ChangelogDlg::CHANGELOG_PACKAGE, QUtil::usr_dir( QUtil::eUsrDir::Changelog
), ChangelogDlg::CSS_FILE ); if( res ) retval = true;
          }
        }
        RemoveDir( tmp_dir_path );
      }
      else
      {
        printer->error( "tmp dir is not writable or you don't have enought rights : '%s'", dir_qs.toStdString().c_str()
);
      }
    }
    else
    {
      printer->error( "tmp dir doesn't exist : '%s'", dir_qs.toStdString().c_str() );
    }
  }
  else
  {
    printer->error( "not found: '%s'", ChangelogDlg::CHANGELOG_PACKAGE.c_str() );
  }
  return retval;
}*/