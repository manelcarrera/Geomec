// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_COMMAND_H
#define RG_COMMAND_H

#include "RGDate.h"
#include "RGFailure.h"
#include "RGInterfaceExport.h"
#include "RGProperty.h"

///////////////////////////////////////////////////////////////////////////////
/// All commands handled by GEOMECH
///////////////////////////////////////////////////////////////////////////////
enum GMCommandType {
  typeCommandFirst = 0,
  typeCommandLoadGrid = 0,
  typeCommandSaveGrid,

  typeCommandLoadProperty,
  typeCommandSaveProperty,

  typeCommandCalculate,
  typeCommandDefineInitialDate,
  typeCommandQuit,

  typeCommandHibernate,

  typeCommandListAvailableFailureMechanisms,
  typeCommandEvaluateLimitStateFunctions,

  typeCommandListMonitorableProperties,
  typeCommandLoadMonitorPointSets,
  typeCommandEvaluateMonitorValues,

  typeCommandLast
};

///////////////////////////////////////////////////////////////////////////////
/// Base class for various load/save commands
///////////////////////////////////////////////////////////////////////////////

class INTERFACE_RGINTERFACE GMCommand {
public:
  /// @brief Default empty constructor, used for constructing GMCommand arrays mostly. Creates invalid object
  GMCommand() : m_command(typeCommandLast) { ; }

  /// @brief Copy constructor
  /// @param command - object to being copied
  GMCommand(const GMCommandType &command) : m_command(command) { ; }

  /// @brief Constructor of command for load/save a property
  /// @param command to load or save given property
  GMCommand(const GMCommandType &command, const RGProperty &property) : m_command(command), m_property(property) { ; }

  /// @brief Constructor of command for setting the initial date for calculation
  /// @param date - date object which defines year, month and day
  GMCommand(const GMCommandType &command, const RGDate &date) : m_command(command), m_date(date) { ; }

  /// @brief Get tyep of the command
  /// @return what type of the command is
  GMCommandType getType() const { return m_command; }

  /// @brief Get property associated with this command. The command type must be typeCommandSaveProperty or
  /// typeCommandLoadProperty
  /// @return property object
  RGProperty getProperty() const {
    assert(m_command == typeCommandSaveProperty || m_command == typeCommandLoadProperty);
    return m_property;
  }

  /// @brief Get date object associated with this command. The command type must be typeCommandDefineInitialDate
  /// @return date object
  RGDate getDatePrm() const {
    assert(m_command == typeCommandDefineInitialDate);
    return m_date;
  }

  /// @brief Convert current command to the string
  /// @return string representation on the current command
  std::string toStringCmd() const;

  /// @brief Convert command parameters to the string
  /// @return string representation of the command parameters
  std::string toStringPrms() const;

  /// @brief Construct GMCommand object from string representations of it name and parameters
  /// @param cmd string representation of command
  /// @param prms string representation of command parameters
  /// @return constructed GMCommand object. If command name or command parameters can't be recognized, it returns
  /// invalid object
  static GMCommand fromStrings(const std::string &cmd, const std::string &prms);

  /// @brief Do checking is current GMCommand object is valid
  /// @return true if object was correctly constructed, false otherwise
  bool isValid() const { return m_command != typeCommandLast; }

  /// @brief Comparison for equality of two GMCommand objects
  /// @param tcm other object for comparing
  /// @return false if any field in tcm object is different from the same field in current object, true otherwise
  bool operator==(const GMCommand &tcm) const;

  /// @brief Comparison for unequality of two GMCommand objects
  /// @param tcm other object for comparing
  /// @return true if any field in tcm object is different from the same field in current object, false otherwise
  bool operator!=(const GMCommand &tcm) const { return !(*this == tcm); }

private:
  GMCommandType m_command; /// command type
  RGProperty m_property;   /// property parameter (if it has) of the command
  RGDate m_date;           /// some date
};

#endif
