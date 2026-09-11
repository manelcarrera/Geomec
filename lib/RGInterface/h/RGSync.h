// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_SYNC_H
#define RG_SYNC_H

#include "RGInterfaceExport.h"

class RGLock;

#include <string>

///////////////////////////////////////////////////////////////////////////////
/// Sync the two parties that use the RGInterface communication file.
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGSync {
public:
  enum ProcessRole { Lead, Follower };

  /// @brief Constructor of the sync mechanism.
  /// @param role provide the mode the sync is running.
  /// @param modelName provide the location of the sync file or
  /// the name of the semaphore.
  RGSync(ProcessRole role, const std::string &modelName);

  ~RGSync();

  /// @brief Initialize the sync mechanism.
  void init();

  /// @brief lock the resource.
  void lock();

  /// @brief release all the resources so the other process can continue.
  void release();

  /// @brief wait for the other process to finish.
  /// @param timeout number of seconds the method will wait. If -1 is provided
  ///        then the process waits forever.
  /// @return true if lock is given free. false if the timeout has expired.
  bool waitFor(int timeOut);

private:
  RGLock *m_lock;
};

#endif // RG_SYNC_H
