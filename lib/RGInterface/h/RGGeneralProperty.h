// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#ifndef RG_GENERAL_PROPERTY_H
#define RG_GENERAL_PROPERTY_H

#include "RGInterfaceExport.h"

#include <string>


///////////////////////////////////////////////////////////////////////////////
/// General property, as opposed to RGProperty that is defined by a fixed string.
///////////////////////////////////////////////////////////////////////////////

namespace RGSupport
{
   ///////////////////////////////////////////////////////////////////////////////
   /// Topological support of property
   ///////////////////////////////////////////////////////////////////////////////
   enum Support
   {
       InvalidSupport = -1, Element = 0, Node, IntegrationPoint
   };

   inline std::string toString(Support support)
   {
	   std::string retValue;
	   switch (support)
	   {
	   case RGSupport::Element:
		   retValue = "Element";
		   break;
	   case RGSupport::Node:
		   retValue = "Node";
		   break;
	   case RGSupport::IntegrationPoint:
		   retValue = "IntegrationPoint";
		   break;
	   default:
		   retValue = "";
		   break;
	   }

	   return retValue;
   }

   inline Support toEnum(std::string support)
   {
	   if (support == "Element")
		   return Element;
	   else if (support == "Node")
		   return Node;
	   else if (support == "IntegrationPoint")
		   return IntegrationPoint;
	   else
		   return InvalidSupport;
   }
}

///////////////////////////////////////////////////////////////////////////////
/// @class RGGeneralProperty
/// @brief General property description
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGGeneralProperty
{
public:

	RGGeneralProperty(const std::string &propName, const std::string &quant, const RGSupport::Support &support);

	std::string GetProperty() const;
	std::string GetQuantity() const;
	RGSupport::Support GetSupport() const;

private:
	std::string m_propName;
	std::string m_quantity;
	RGSupport::Support m_support;

};

#endif


