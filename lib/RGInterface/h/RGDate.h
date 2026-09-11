// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_DATE_H
#define RG_DATE_H

#include "RGInterfaceExport.h"
#include <cassert>
#include <string>

///////////////////////////////////////////////////////////////////////////////
/// Class for keeping date
/////////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGDate {
public:
  /// Default constructor
  RGDate() : m_year(1960), m_month(8), m_day(4) { ; }

  /// Constructor with parameter verification
  RGDate(int year, int month, int day) : m_year(year), m_month(month), m_day(day) {
    assert(year >= -4800);
    assert(month >= 0 && month < 13);
    assert(day >= 0 && day < 32);
  };

  int getYear() const { return m_year; }   /// Get year of the date
  int getMonth() const { return m_month; } /// Get month of the date
  int getDay() const { return m_day; }     /// Get day of the date

  bool operator==(const RGDate &dt) const {
    return dt.getYear() == m_year && dt.getMonth() == m_month && dt.getDay() == m_day;
  }
  bool operator!=(const RGDate &dt) const { return !(*this == dt); }

  /// @brief Convert date to the string
  /// @return string representation on the date
  std::string toString() const;

  /// @brief Construct RGDate object from string representation
  /// @param cmd string representation of RGDate object
  /// @return constructed RGDate object. If command name or command parameters can't be recognized, it returns default
  static RGDate fromString(const std::string &dateStr);

private:
  int m_year;
  int m_month;
  int m_day;
};

#endif
