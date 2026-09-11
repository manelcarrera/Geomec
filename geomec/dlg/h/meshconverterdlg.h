#pragma once

#include <QtWidgets\QDialog>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QKeyEvent;

class MeshConverterTreeView;

class MeshConverterDlg : public QDialog
{
  Q_OBJECT

  QLabel* lb1;
  QPushButton* b1;

  QVBoxLayout* lv;
  QHBoxLayout* lh;

  MeshConverterTreeView* m_tree;

public slots:

  void help();

private:

  MeshConverterDlg();
  ~MeshConverterDlg();

  void update();
  void keyPressEvent(QKeyEvent *e);

public:

  static MeshConverterDlg* instance();
};
