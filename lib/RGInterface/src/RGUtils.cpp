// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGDepletionStage.h"
#include "RGUtils.h"

#include <cassert>
#include <RescueModel.h>


///////////////////////////////////////////////////////////////////////////////
/// @brief Get the grid in the Rescue Model from where a propriety exists
/// @param pModel rescue model
/// @param propertyName
/// @return a grid containing the property, or null if property not found
///////////////////////////////////////////////////////////////////////////////
RescueGeometry* RGUtils::getRescueGridByProperty( std::unique_ptr<RescueModel> & pModel, const std::string& propertyName )
{
   RescueGeometry * pGrid = NULL;
   for (int id = 1; id < pModel->GeometryCount(); ++id)
   {
      if ( pModel->GridGeometry( id )->PropertyNamed( const_cast<char *>(propertyName.c_str()) ) )
      {
         pGrid = pModel->GridGeometry( id );
      }
   }
   return pGrid;
}
