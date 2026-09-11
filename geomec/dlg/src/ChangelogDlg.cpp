#include "stdafx.h"

#include "ChangelogDlg.h"

#include "GeomecUtils.h"
#include "Plugins.h"
#include "Printer.h"
#include "QUtil.h"
#include "Xml.h"
#include "util_.h"

const std::string ChangelogDlg::CHANGELOG_DIR = QUtil::native_sepatators(util::GetAppPath() + "/changelog");
const std::string ChangelogDlg::GEOMEC_CONFIG_FILE =
    QUtil::native_sepatators(std::string(getenv("APPDATA")) + "/Geomec/geomec.xml");
const std::string ChangelogDlg::CHANGELOG_PACKAGE =
    QUtil::native_sepatators(GetAppPath().toStdString() + "/packages/changelog.zip");
const std::string ChangelogDlg::CSS_FILE = "changelog.css";

void ChangelogDlg::reject() {
  std::string gm_cfg_file = QDir::toNativeSeparators(QString::fromStdString(GEOMEC_CONFIG_FILE)).toStdString();

  if (check.isChecked())
    CXml::update(gm_cfg_file, "show", "no");

  QDialog::reject();
}

ChangelogDlg::ChangelogDlg() {
  resize(800, 600);

  std::string changelog_dir = QDir::toNativeSeparators(QString::fromStdString(CHANGELOG_DIR)).toStdString();

  m_changelog_v = QUtil::folders(changelog_dir, QUtil::eFile::Name);

  for (int i = 0; i < m_changelog_v.size(); i++) {
    combo.addItem(QString::fromStdString(m_changelog_v[i]));
    Printer::instance()->debug("%d: %s", i, m_changelog_v[i].c_str());
  }

  QObject::connect(&combo, SIGNAL(currentIndexChanged(int)), this, SLOT(change_idx(int)));
  combo.setCurrentIndex(m_changelog_v.size() - 1);

  check.setText("Don't show again");

  layout.addWidget(&combo);
  layout.addWidget(&browser);
  layout.addWidget(&check);

  this->setLayout(&layout);

  if (m_changelog_v.size() == 1)
    doc(m_changelog_v[0]);
}

void ChangelogDlg::doc(std::string file_) {
  std::string file_s = QUtil::native_sepatators(CHANGELOG_DIR + "\\" + file_ + "\\" + "index.html");
  std::string img_path = QUtil::native_sepatators(CHANGELOG_DIR + "\\" + file_);
  // std::string css_file	= QUtil::native_sepatators( CHANGELOG_DIR + "\\" + CSS_FILE );

#ifdef Q_WS_WIN // WebKit
  browser.document()->setMetaInformation(QTextDocument::DocumentUrl, img_path + "/");
#else // QTextBrowser
  browser.document()->setMetaInformation(
      QTextDocument::DocumentUrl,
      QString::fromStdString("file:" + img_path + "/")); // Attention: don't play with this last slash: '/'
#endif

  QFile file(QDir::toNativeSeparators(QString::fromStdString(file_s)));
  file.open(QFile::ReadOnly | QFile::Text);
  QTextStream stream(&file);
  browser.setHtml(stream.readAll());
}

void ChangelogDlg::change_idx(int idx) { doc(m_changelog_v[idx]); }
bool ChangelogDlg::verify() { return m_changelog_v.size(); }