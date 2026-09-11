// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGFileLock.h"
#include "RGFileLockImpl.h"
#include "RGSync.h"

#include <time.h>

RGFileLock::RGFileLock(RGSync::ProcessRole role, const std::string& modelName) :
  RGLock(role, modelName)
{
   m_impl = RGFileLockImpl::createSyncLock( modelName );
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Destructor
///////////////////////////////////////////////////////////////////////////////
RGFileLock::~RGFileLock(void)
{
   if (role() == RGSync::Lead)
   {
      if (m_impl->exists())
      {
         // remove the special file:
         m_impl->remove();
      }
   }
   delete m_impl;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize the locking mechanism.
///////////////////////////////////////////////////////////////////////////////
void RGFileLock::init()
{
   if (role() == RGSync::Lead)
   {
      if (m_impl->exists())
      {
         // remove the special file:
         m_impl->remove();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Lock the resource.
///////////////////////////////////////////////////////////////////////////////
void RGFileLock::lock()
{
   // no implementation.
}

///////////////////////////////////////////////////////////////////////////////
/// @brief release all the resources so the other process can continue.
///////////////////////////////////////////////////////////////////////////////
void RGFileLock::release()
{
   // no implementation.
   if (role() == RGSync::Lead)
   {
       m_impl->create();
   }
   else
   {
      if (m_impl->exists())
      {
         // remove the special file.
         m_impl->remove();
      }   
   }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief wait for other process.
/// @param timeout in seconds. If the timeout is set on -1 there will be no
///        timeout and the method will wait until the lock becomes avaialble.
///////////////////////////////////////////////////////////////////////////////
bool RGFileLock::waitFor(int timeout)
{
   bool isProcessReady = false;
   bool ok = true;
   time_t now;
   time_t start;

   // the begin time.
   time(&start);

   while(!isProcessReady && ok)
   {
      if (role() == RGSync::Lead)
      {
         isProcessReady = !m_impl->exists();
      }
      else
      {
         isProcessReady = m_impl->exists();
      }

      if (!isProcessReady)
      {
         if (timeout == -1)
         {
      m_impl->wait();
         }
         else
         {
      // get the current time so we can
      // determine if we have waited long enough.
      time(&now);
      double diff = difftime(now, start);
      if (diff > timeout) 
      {
               ok = false;
      }
      else
      {
               m_impl->wait();
      }
         }    
      }
   }
   return ok;
}



