// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#ifndef RG_DEPLETION_STAGE_H
#define RG_DEPLETION_STAGE_H

#include "RGInterfaceExport.h"

#include <cassert>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
/// Initialization: depletionStage = 0
///////////////////////////////////////////////////////////////////////////////
#define RG_INITIAL_STAGE 0


///////////////////////////////////////////////////////////////////////////////
/// Depletion stage + time 
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGDepletionStage
{
public:
   /// @brief Copy constructor
   /// @param ds RGDepletionStage object to be copying
   RGDepletionStage( const RGDepletionStage & ds ) : 
      m_elapsedSecs( ds.getElapsedSecs() ), m_stage( ds.getDepletionStage() ) {;}

   /// @brief Constructor
   /// @param depletionStage depletion stage number must be >= RG_INITIAL_STAGE
   /// @param elapsedSecs number of elapsed seconds from previous depletion stage
   RGDepletionStage( const int & depletionStage, const int & elapsedSecs ) : 
      m_elapsedSecs( elapsedSecs ),
      m_stage( depletionStage )
   { 
      assert( depletionStage >= RG_INITIAL_STAGE );
      assert( elapsedSecs > -1 ); // can't go back in time 
   }

   /// @brief Default constructor, creates unsetted object, isValid will return false for uninitialised objects
   RGDepletionStage() : m_elapsedSecs( -1 ), m_stage( RG_INITIAL_STAGE - 1 ) {;}

   /// @brief Get depletion stage number
   /// @return depletion stage number
   int getDepletionStage() const { assert( isValid() ); return m_stage; }
  
   /// @brief Get year for current depletion stage
   /// @return year
   int getElapsedSecs()  const { assert( isValid() ); return m_elapsedSecs; }

   /// @brief Comparison for equality of two RGDepletionStage objects
   /// @param ds other object for comparing
   /// @return true if all fields in ds the same as in current object, false otherwise
   bool operator == ( const RGDepletionStage & ds ) const
   { 
      return m_stage == ds.getDepletionStage(); 
   }

   /// @brief Comparison for unequality of two RGDepletionStage objects
   /// @param ds other object for comparing
   /// @return false if all fields in ds the same as in current object, true otherwise
   bool operator != ( const RGDepletionStage & ds ) const  { return !(*this == ds ); }

   /// @brief Compare if given depletion stage is early then current
   /// @param ds other object for comparing
   /// @return true if current object is later in depletion stage and in time then given object
   bool operator > ( const RGDepletionStage & ds ) const
   {
      return m_stage > ds.getDepletionStage();
   }

   /// @brief Compare if given depletion stage is later then current
   /// @param ds other object for comparing
   /// @return true if current object is later in depletion stage and in time then given object
   bool operator < ( const RGDepletionStage & ds ) const
   {
      return m_stage < ds.getDepletionStage();
   }

  /// @brief check is RGDepletionStage object was initialised
   /// @return true if RGDepletionStage object was defined correctly, false otherwise
   bool isValid() const { return m_stage >= RG_INITIAL_STAGE; }

   /// @brief Convert depletion stage to string
   /// @return string representation of RGDepletionStage
   std::string toString() const;

   /// @brief Construct RGDepletionStage object from string
   /// @param str string which were obtained through toString call
   /// @return empty object if string is not in correct format, or rightfully constructed object
   static RGDepletionStage fromString( const std::string & str );

private:
   int m_elapsedSecs;
   int m_stage;
};

///////////////////////////////////////////////////////////////////////////////
/// @brief  Formated printing of RGDepletionStage as "(stage, month-year)"
/// @param out output stream
/// @param ds depeletion stage object
/// @return output stream
///////////////////////////////////////////////////////////////////////////////
std::ostream & operator << ( std::ostream & out, const RGDepletionStage & ds );
#endif

