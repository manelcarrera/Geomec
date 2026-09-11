#pragma once

#include <QObject>
#include <QPushButton>
#include <QString>

class CBtn : public QPushButton {
  Q_OBJECT

public:
  enum eAction { Minus = -1, Plus = 1 };
  int m_idx;
  eAction m_action;
  CBtn(const QString text, int idx, eAction action) : QPushButton(text), m_idx(idx), m_action(action) {}
};