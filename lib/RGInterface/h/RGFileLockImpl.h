// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_FILELOCKIMPL_H
#define RG_FILELOCKIMPL_H

#include <string>

///////////////////////////////////////////////////////////////////////////////
/// Abstraction of the lock actions.
///////////////////////////////////////////////////////////////////////////////
class RGFileLockImpl {
public:
  /// @brief Factory method.
  static RGFileLockImpl *createSyncLock(const std::string &name);

  /// @brief destructor.
  virtual ~RGFileLockImpl() {}

  /// @brief Create the lock.
  virtual void create() = 0;

  /// @brief Does the lock exists?
  /// @return true if the lock exists.
  virtual bool exists() = 0;

  /// @brief Remove the lock.
  virtual void remove() = 0;

  /// @brief Wait until resource is free.
  virtual void wait() = 0;

protected:
  RGFileLockImpl(const std::string &name);

  std::string m_name;
};

#endif
