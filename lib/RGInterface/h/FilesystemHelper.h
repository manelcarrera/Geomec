// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_FILESYSTEM_HELPER_H
#define RG_FILESYSTEM_HELPER_H

#include "RGInterfaceExport.h"
#include <string>

namespace FilesystemHelper
{
   std::string INTERFACE_RGINTERFACE DirName2ModelName( const std::string & modelName );
   std::string INTERFACE_RGINTERFACE RGGetCWD();
   bool        INTERFACE_RGINTERFACE IsFullPath( const std::string & somePath );
   std::string INTERFACE_RGINTERFACE RGGetFileName( std::string modelName, bool isTxtFormat = false );   
   std::string INTERFACE_RGINTERFACE RGGetDirName( const std::string & fileName );
   void        INTERFACE_RGINTERFACE mkdir( const std::string& );
   std::string INTERFACE_RGINTERFACE getProcessName();
   void        INTERFACE_RGINTERFACE sleepFor( unsigned int milliSeconds );
}

#endif
