// Copyright 2014, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_MATERIAL_TYPE
#define RG_MATERIAL_TYPE

#include "RGInterfaceExport.h"

namespace RGMaterialType
{
   ///////////////////////////////////////////////////////////////////////////////
   /// @brief Enumeration for material types
   ///////////////////////////////////////////////////////////////////////////////
   enum Type
   {
      UNKNOWN,
      LINEAR             ,  // Linear                                                              
      CAMCLAY            ,  // Cam-clay                                                            
      MOHRCOULOMB        ,  // Mohr-Coulomb                                                        
      MODIFIEDMOHRCOULOMB,  // Modified Mohr-Coulomb                                               
      CREEP              ,  // Salt Creep                                                          
      UNDRAINED          ,  // Undrained Elastic                                                   
      MC_COHESION_HARD1  ,  // Mohr-Coulomb with Linear Cohesion Hardening/Softening               
      MC_COHESION_HARD2  ,  // Mohr-Coulomb with Bi-linear Cohesion Hardening/Softening            
      MC_COHESION_HARD3  ,  // Mohr-Coulomb with Tri-linear Cohesion Hardening/Softening           
      MC_FRICTION_HARD1  ,  // Mohr-Coulomb with Linear Friction Angle Hardening/Softening         
      MC_FRICTION_HARD2  ,  // Mohr-Coulomb with Bi-linear Friction Angle Hardening/Softening      
      MC_FRICTION_HARD3  ,  // Mohr-Coulomb with Tri-linear Friction Angle Hardening/Softening     
      RIGIDITY           ,  // General Anisotropy                                                  
      DUALCAP_LINELA     ,  // Dual Cap with linear elasticity                                     
      FRACTURE_ANISOTROPY,  // Fracture Anisotropy                                                 
      UPSCALED_ANISOTROPY,  // Upscaled Anisotropy                                                 
      FRACTURE_APERTURE  ,  // Fracture Aperture                                                   
      ANISOTROPIC_CAMCLAY,  // Anisotropic Cam-clay                                                
      FRACTURE_APERTURE2    // Fracture Aperture 2                                                 
   };

}

#endif
