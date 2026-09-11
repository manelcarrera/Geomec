// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_LOCK_H
#define RG_LOCK_H

#include "RGInterfaceExport.h"
#include "RGSync.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////
/// Base class for lock implementation.
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGLock
{
public:

   /// @brief Constructor of the sync mechanism.
   /// @param role provide the mode the sync is running.
   /// @param modelName provide the location of the sync file.
   RGLock( RGSync::ProcessRole role, const std::string& modelName );

   /// @brief destructor.
   virtual ~RGLock();

   /// @brief return the mode of this lock.
   RGSync::ProcessRole role() const;

   /// @brief return the model name.
   const std::string model() const;
   
   /// @brief Initialize the sync mechanism. 
   virtual void init() = 0;

   /// @brief lock the resource.
   virtual void lock() = 0;

   /// @brief release all the resources so the other process can continue.
   virtual void release() = 0;

   /// @brief wait for the other process to finish.
   /// @param timeout number of seconds the method will wait.
   /// @return true if lock is given free. false if the timeout has expired.
   virtual bool waitFor(int timeout) = 0;

private:
   RGSync::ProcessRole m_role;
   std::string* m_modelName; // must use a pointer for DLL export
};

#endif // RG_SYNC_H

