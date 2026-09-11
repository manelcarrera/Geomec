#pragma once

#include <QString>

#include "Progress_.h" //eProgress

class CProgressData {
public:
  enum eCmd { New, Delete, AddSteps_, Step_, Status_, NextJob_, Enable_, Current, Cancel };
  static const char *cmd_cp[]; // FIXME: initialized in ProgressWrapper_Evt

public:
  eCmd m_cmd;
  eProgress m_type;
  int m_job_idx;
  int m_number;
  QString m_msg;
  bool m_yes_no; // bCancel, Enable

  //
  // Specific constructors, just for convenience
  //

  // New
  CProgressData(eCmd cmd, eProgress type = eProgress::Geo, int job_idx = 0, int number = 1, QString msg = "",
                bool bCancel = true)
      : m_cmd(cmd), m_type(type), m_job_idx(job_idx), m_number(number), m_msg(msg), m_yes_no(bCancel) {}

  // AddSteps, Step
  CProgressData(eCmd cmd, int number) : m_cmd(cmd), m_number(number) {}

  // Enable
  CProgressData(eCmd cmd, bool bEnable) : m_cmd(cmd), m_yes_no(bEnable) {}

  // Status, NextJob
  CProgressData(eCmd cmd, QString msg) : m_cmd(cmd), m_msg(msg) {}
};