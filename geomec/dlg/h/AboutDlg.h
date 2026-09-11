#pragma once

#include <QDialog>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class QAboutDlg : public QDialog {
  Q_OBJECT

  QVBoxLayout m_v_layout;
  QHBoxLayout m_h_layout_up;
  QVBoxLayout m_v_layout_up_right;

  QHBoxLayout m_h_layout_new;

  QVBoxLayout m_v_layout_more;
  QVBoxLayout m_v_layout_more_2;

  QHBoxLayout m_h_layout_arrows;
  QHBoxLayout m_h_layout_arrows_2;

  QPushButton m_btn_down;
  QPushButton m_btn_down_2;

  QPushButton m_btn_up;
  QPushButton m_btn_up_2;

  QLabel m_lb_version;
  QLabel m_lb_dir;

  QPushButton m_btn_new;  // img
  QPushButton m_btn_new2; // text

  QPushButton m_btn_menu_pdf;
  QPushButton m_btn_menu_var;

  QPushButton m_btn_logo;

  enum eText {
    Version,
    Working_Dir,
    Gm_Default_Version,
    App_Last_Model_Version,
    Config_File,
    // diana
    Diapath_Current,
    Diapath_Env,
    Diapath_Config_File,
    Diapath_Force_Local
  };

  QStringList m_pdfs;

private:
  void add_pdfs();
  void add_texts();
  QString text(eText type_);

public slots:
  void on_changelog();

  void on_more();
  void on_less();

  void on_more_2();
  void on_less_2();

  void on_open_pdf(int i);

public:
  QAboutDlg(QDialog *parent = nullptr);
  ~QAboutDlg();
};
