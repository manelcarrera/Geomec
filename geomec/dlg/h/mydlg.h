#ifndef MYDLG_H
#define MYDLG_H

#include <QtWidgets\QDialog>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QCheckBox;

class MyDlg : public QDialog {
  Q_OBJECT

  QLabel *lb1;
  QPushButton *b2;
  QPushButton *b3;

  QCheckBox *cb;

  QVBoxLayout *lv;
  QHBoxLayout *lh;

public slots:

  void yes();
  void no();
  void cb_(int state);

public:
  MyDlg();
  ~MyDlg();

  void keyPressEvent(QKeyEvent *e);
};

#endif // MYDLG_H
