// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#include "GMCommand.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
std::string GMCommand::toStringCmd() const
{
   switch( m_command )
   {
      case typeCommandLoadGrid:          return "LoadGrid";
      case typeCommandSaveGrid:          return "SaveGrid";
      case typeCommandLoadProperty:      return "LoadProperty";
      case typeCommandSaveProperty:      return "SaveProperty";
      case typeCommandCalculate:         return "Calculate";
      case typeCommandHibernate:         return "Hibernate";
      case typeCommandListAvailableFailureMechanisms: return "ListAvailableFailureMechanisms";
    case typeCommandEvaluateLimitStateFunctions: return "typeCommandEvaluateLimitStateFunctions";
      case typeCommandQuit:              return "Quit";
      case typeCommandDefineInitialDate: return "DefineInitialDate";
    case typeCommandListMonitorableProperties: return "typeCommandListMonitorableProperties";
    case typeCommandLoadMonitorPointSets: return "typeCommandLoadMonitorPointSets";
    case typeCommandEvaluateMonitorValues: return "typeCommandEvaluateMonitorValues";
      case typeCommandLast:         
      default:                           return "Unknown";                                 
   }
}

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
std::string GMCommand::toStringPrms() const
{
   switch( m_command )
   {
      case typeCommandLoadProperty:
      case typeCommandSaveProperty:
         return m_property.isValid() ? m_property.toString() : "";

      case typeCommandDefineInitialDate: 
         return m_date.toString();
         break;
   }
   return ""; 
}
   
///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
GMCommand GMCommand::fromStrings( const std::string & cmd, const std::string & prms )
{
   GMCommandType cmdType = typeCommandLast;

   if (      cmd == "LoadGrid"          ) cmdType = typeCommandLoadGrid;
   else if ( cmd == "SaveGrid"          ) cmdType = typeCommandSaveGrid;
   else if ( cmd == "LoadProperty"      ) cmdType = typeCommandLoadProperty;
   else if ( cmd == "SaveProperty"      ) cmdType = typeCommandSaveProperty;
   else if ( cmd == "Calculate"         ) cmdType = typeCommandCalculate;
   else if ( cmd == "Hibernate"         ) cmdType = typeCommandHibernate;
   else if (cmd == "ListAvailableFailureMechanisms") cmdType = typeCommandListAvailableFailureMechanisms;
   else if (cmd == "typeCommandEvaluateLimitStateFunctions") cmdType = typeCommandEvaluateLimitStateFunctions;
   else if ( cmd == "Quit"              ) cmdType = typeCommandQuit;
   else if ( cmd == "DefineInitialDate" ) cmdType = typeCommandDefineInitialDate;
   else if (cmd == "typeCommandListMonitorableProperties") cmdType = typeCommandListMonitorableProperties;
   else if (cmd == "typeCommandLoadMonitorPointSets") cmdType = typeCommandLoadMonitorPointSets;
   else if (cmd == "typeCommandEvaluateMonitorValues") cmdType = typeCommandEvaluateMonitorValues;
 
   else                                   cmdType = typeCommandLast;
   
   GMCommand retCmd;

   switch( cmdType )
   {
      case typeCommandLoadProperty:
      case typeCommandSaveProperty:
         retCmd = prms.size() ? GMCommand( cmdType, RGProperty::fromString( prms ) ) : GMCommand( cmdType );
         break;

      case typeCommandDefineInitialDate: 
         retCmd = GMCommand( cmdType, RGDate::fromString( prms ) );
         break;

      default:
         retCmd = GMCommand( cmdType );

         break;
   }
   return retCmd;
}

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
bool GMCommand::operator == ( const GMCommand & tcm ) const 
{ 
   if ( m_command != tcm.m_command ) return false;

   switch( m_command )
   {
      case typeCommandLoadProperty:
      case typeCommandSaveProperty:
         return m_property == tcm.m_property;

      case typeCommandDefineInitialDate: 
         return m_date == tcm.m_date;
         break;
   }
   return true;
}


