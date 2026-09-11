// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#include <sstream>

#include "RGDepletionStage.h"
#include "RGInterface.h"

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
std::string RGDepletionStage::toString() const
{
   std::ostringstream out;
   out << m_stage << "-" << m_elapsedSecs;
   return out.str();
}

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
RGDepletionStage RGDepletionStage::fromString( const std::string & str )
{
   try
   {
      std::stringstream s;
      s << str;
      int stgs[2];
      for ( size_t i = 0; i < 3; ++i )
      {
         s >> stgs[i];
         s.ignore( 1 );
      }
      return RGDepletionStage( stgs[0], stgs[1] );
   }
   catch ( std::ios_base::failure& )
   {
      std::ostringstream msg;
      msg << "Depletion stage received \"" << str
          << "\" is not in expected format \"(stage number)-(elapsed seconds)\"";
      throw RGInterface::Exception( msg.str() );
   }
}

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
std::ostream & operator << ( std::ostream & out, const RGDepletionStage & ds )
{
   out << "(" << ds.getDepletionStage() << ", " << ds.getElapsedSecs() << ")";
   return out;
}


