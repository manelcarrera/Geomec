#pragma once

class CArchiveInterface;

#include "RBox.h"

#include <QMap>
#include <QString>

class CMeshRefinementModel {
  TBoxMap m_data_m;
  // QMap< QString, bool > m_visible_m;

public:
  CMeshRefinementModel();

  void to_stream(CArchiveInterface &stream);
  void from_stream(CArchiveInterface &stream);

  TBoxMap &data() { return m_data_m; };
  void data(TBoxMap data_) { m_data_m = data_; };

  TBoxV data_v();

  void print();
  void print(const RefinementBox &box);
};