// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#include "RGDate.h"
#include "RGInterface.h"

#include <string>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
std::string RGDate::toString() const
{
   std::ostringstream ss;
   ss << m_day << "-" << m_month << "-" << m_year;

   return ss.str();
}

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
RGDate RGDate::fromString( const std::string & dateStr )
{
   try
   {
      std::stringstream s;
      s << dateStr;
      int date[3];
      for ( size_t i = 0; i < 3; ++i )
      {
         s >> date[i];
         s.ignore( 1 );
      }
      return RGDate( date[2], date[1], date[0] );
   }
   catch ( std::ios_base::failure& )
   {
      std::ostringstream msg;
      msg << "Date received \"" << dateStr << "\" is not in expected format \"DD-MM-YYYY\"";
      throw RGInterface::Exception( msg.str() );
   }
}


