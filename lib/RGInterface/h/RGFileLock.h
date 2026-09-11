// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_FILELOCK_H_
#define RG_FILELOCK_H_

#include "RGInterfaceExport.h"
#include "RGLock.h"
#include "RGSync.h"

#include <string>

class RGFileLockImpl;

///////////////////////////////////////////////////////////////////////////////
/// Sync the two parties that use the RGInterface communication file.
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGFileLock : public RGLock
{
public:

   /// @brief Constructor of the sync mechanism.
   /// @param role provide the mode the sync is running.
   /// @param modelName provide the location of the sync file.
   RGFileLock( RGSync::ProcessRole role, const std::string& modelName );

   virtual ~RGFileLock();

   /// @brief Initialize the sync mechanism. 
   virtual void init();

   /// @brief Lock the file.
   virtual void lock();

   /// @brief release all the resources so the other process can continue.
   virtual void release();

   /// @brief wait for the other process to finish.
   /// @param timeout number of seconds the method will wait.
   /// @return true if lock is given free. false if the timeout has expired.
   virtual bool waitFor(int timeout);

private:
   
   RGFileLockImpl* m_impl;
};

#endif // RG_FILELOCK_H_

