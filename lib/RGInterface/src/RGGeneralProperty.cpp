// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGGeneralProperty.h"
#include "RGQuantity.h"

#include <stdexcept>
#include <string>

///////////////////////////////////////////////////////////////////////////////
/// @class RGGeneralProperty
/// @brief General property description
///////////////////////////////////////////////////////////////////////////////

RGGeneralProperty::RGGeneralProperty(const std::string &propName, const std::string &quant,
                                     const RGSupport::Support &support) {
  if (propName == "" || propName.find(' ') != std::string::npos || !RGQuantity::IsKnownQuantity(quant)) {
    throw std::runtime_error("RGGeneralProperty() - Recieved Invalid Input");
  }

  m_propName = propName;
  m_quantity = quant;
  m_support = support;
}

std::string RGGeneralProperty::GetProperty() const { return m_propName; }

std::string RGGeneralProperty::GetQuantity() const { return m_quantity; }

RGSupport::Support RGGeneralProperty::GetSupport() const { return m_support; }
