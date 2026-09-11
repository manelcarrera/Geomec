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

#ifndef __LICENSERESOURCE_H__
#define __LICENSERESOURCE_H__

#include "resource.h"

#ifdef _LICENSERESOURCEDLL
#define AFX_EXT_CLASS_LICENSERESOURCE __declspec(dllexport)
#else
#define AFX_EXT_CLASS_LICENSERESOURCE __declspec(dllimport)
#endif

#ifdef _DEBUG
#define LICENSERESOURCE_DLL_NAME "LicenseResourceD.dll"
#else
#define LICENSERESOURCE_DLL_NAME "LicenseResource.dll"
#endif

#endif
