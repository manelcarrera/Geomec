/*****************************************************************
 * FILENAME:
 *   $Workfile: InitResource.h $
 *
 * PURPOSE:
 *   This file defines datamanager specific items
 *
 * COPYRIGHT 2010 Shell International Exploration and Production
 * bv Rijswijk, The Netherlands.
 *
 * CONFIDENTIALITY:
 *   This is a CONFIDENTIAL document.
 *   Neither the  whole  or any  part of this document may
 *   be  reproduced,  stored in  any retrieval  system  or
 *   transmitted in any form or by any means  (electronic,
 *   mechanical,  reprographic,  recording  or  otherwise)
 *   without the  prior  consent of the  copyright  owner.
 *
 * VERSION INFORMATION:
 *   $Revision:  $
 *   $Author:  $
 *   $Date: $
 *
 * PART OF MODULE:
 *   LicensResource
 *
 * MODULE CONTROLLER:
 *    Logica
 *****************************************************************/
#pragma once

#include "LicenseResource.h"

// This function should be called from the application to ensure that the LicenseResource dll is loaded.
void AFX_EXT_CLASS_LICENSERESOURCE InitResource();
