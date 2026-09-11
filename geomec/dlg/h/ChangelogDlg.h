#pragma once

#include <QObject>
#include <QTextBrowser>
#include <qdialog>

#include <QVBoxLayout>

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QTextStream>

class ChangelogDlg : public QDialog {
  Q_OBJECT

  static const std::string CHANGELOG_DIR;

  std::vector<std::string> m_changelog_v;

  QTextBrowser browser;
  QComboBox combo;
  QVBoxLayout layout;
  QCheckBox check;

public:
  static const std::string GEOMEC_CONFIG_FILE;
  static const std::string CHANGELOG_PACKAGE;
  static const std::string CSS_FILE;

public slots:
  void change_idx(int idx);

private:
  void doc(std::string file_);
  void reject();

public:
  ChangelogDlg();
  bool verify();
};
