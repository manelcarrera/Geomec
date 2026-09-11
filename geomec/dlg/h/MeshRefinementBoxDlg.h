#pragma once

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

class CMeshRefinementBox;

class CMeshRefinementBoxDlg : public QDialog {
  Q_OBJECT

public:
  static CMeshRefinementBox *m_box;
  // CBox* m_box;

public:
  enum eVals { Position, Rotation, Scale };

private:
  CMeshRefinementBoxDlg(QWidget *parent = 0);
  // CMeshRefinementBox( CBox* box, QWidget *parent = 0 );

  ~CMeshRefinementBoxDlg();

private:
  void update_fields();
  float val(int idx);
  std::vector<float> vals(eVals vals);
  void val(int idx, float val);
  void val(int idx, int val);
  int step(int idx);
  void step(int idx, int val);

public:
  static CMeshRefinementBoxDlg *instance();

  void box(CMeshRefinementBox *box);
  CMeshRefinementBox *box();

  void exec_();
  bool is_show();

  enum ePos { Pe, Pn, Pd };
  enum eRot { Rx = Pd + 1, Ry, Rz };
  enum eScale { Sx = Rz + 1, Sy, Sz };
  enum eCoord { X, Y, Z };

private:
  std::map<int, QLineEdit *> m_edits;
  std::map<int, QLabel *> m_labels;
  std::vector<QLabel *> m_labels_2;
  std::map<int, QPushButton *> m_btns_m;
  std::map<int, QPushButton *> m_btns_p;
  std::map<int, QLineEdit *> m_steps;

  std::vector<QPushButton *> m_btns;

private slots:

  void translate();
  void scale();
  void rotate();

  void btn();
};
