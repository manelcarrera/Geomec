// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGFileLockImpl.h"
#include "FilesystemHelper.h"

#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <stdio.h>
#include <sys/stat.h>  // For stat().
#include <sys/types.h> // For stat().
#ifndef WIN32
#include <unistd.h>
#else
#include <direct.h>
#include <io.h> // For access().
#include <windows.h>
#endif

namespace {
const unsigned int Thousand = 1000;
const int NumberOfMSecsToWait = 1 * Thousand;

///////////////////////////////////////////////////////////////////////////////
/// @brief Sleep for a number of milli seconds.
/// @param milliSconds The number of milli seconds to sleep.
///////////////////////////////////////////////////////////////////////////////
void sleepFor(unsigned int milliSeconds) {
#ifndef WIN32
  for (; (milliSeconds / Thousand) > 0; milliSeconds -= Thousand) {
    usleep(Thousand * Thousand);
  }

  if (milliSeconds > 0) {
    usleep(Thousand * milliSeconds);
  }
#else  // WIN32
  Sleep(milliSeconds);
#endif // WIN32
}

} // anonymous namespace

#ifndef WIN32

///////////////////////////////////////////////////////////////////////////////
/// @brief Linux implementation of the SyncLock. The implementation
/// uses folders as lock elements.
///////////////////////////////////////////////////////////////////////////////
class RGSystemSyncLock : public RGFileLockImpl {
public:
  RGSystemSyncLock(const std::string &fileName) : RGFileLockImpl(fileName) {}

  ///////////////////////////////////////////////////////////////////////////////
  /// @brief create the folder if it not exists.
  ///////////////////////////////////////////////////////////////////////////////
  virtual void create() {
    if (!exists()) {
      int result = mkdir(m_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (result == -1) {
        std::string errorMessage = strerror(errno);
        throw std::runtime_error("Unable to create lock :" + m_name + ". Error: " + errorMessage);
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  /// @brief check if the folder exists.
  ///////////////////////////////////////////////////////////////////////////////
  virtual bool exists() {
    bool ok = false;
    if (access(m_name.c_str(), 0) == 0) {
      struct stat status;
      int res = stat(m_name.c_str(), &status);

      ok = res || (status.st_mode & S_IFDIR) != 0;
    }

    return ok;
  }

  ///////////////////////////////////////////////////////////////////////////////
  /// @brief remove the folder.
  ///////////////////////////////////////////////////////////////////////////////
  virtual void remove() {
    int result = rmdir(m_name.c_str());
    if (result == -1) {
      std::string errorMessage = strerror(errno);
      throw std::runtime_error("can not remove the lock: " + m_name + ". Error: " + errorMessage);
    }
  }

  /// @brief Wait
  virtual void wait() { sleepFor(NumberOfMSecsToWait); }
};

#else

///////////////////////////////////////////////////////////////////////////////
/// @brief Windows implementation of the SyncLock. The implementation
/// uses folders as lock elements.
///////////////////////////////////////////////////////////////////////////////
class RGSystemSyncLock : public RGFileLockImpl {
public:
  RGSystemSyncLock(const std::string &fileName) : RGFileLockImpl(fileName) {}

  ///////////////////////////////////////////////////////////////////////////////
  /// @brief create the folder.
  ///////////////////////////////////////////////////////////////////////////////
  virtual void create() {
    if (!exists()) {
      int result = _mkdir(m_name.c_str());
      if (result == -1) {
        std::string errorMessage = strerror(errno);
        throw std::runtime_error("Unable to create lock :" + m_name + ". Error: " + errorMessage);
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  /// @brief check if the folder exists.
  ///////////////////////////////////////////////////////////////////////////////
  virtual bool exists() {
    bool ok = false;
    if (_access(m_name.c_str(), 0) == 0) {
      struct stat status;
      stat(m_name.c_str(), &status);

      ok = (status.st_mode & S_IFDIR) != 0;
    }

    return ok;
  }

  ///////////////////////////////////////////////////////////////////////////////
  /// @brief remove the folder.
  ///////////////////////////////////////////////////////////////////////////////
  virtual void remove() {
    int result = _rmdir(m_name.c_str());
    if (result == -1) {
      std::string errorMessage = strerror(errno);
      throw std::runtime_error("can not remove the lock: " + m_name + ". Error: " + errorMessage);
    }
  }

  /// @brief Wait
  virtual void wait() { sleepFor(NumberOfMSecsToWait); }
};

#endif

///////////////////////////////////////////////////////////////////////////////
/// @brief Creates a platform dependent sync-file object.
///////////////////////////////////////////////////////////////////////////////
RGFileLockImpl *RGFileLockImpl::createSyncLock(const std::string &modelName) {
  // convert the model name to a file name.
  std::string rgSyncToken = FilesystemHelper::RGGetFileName(modelName);
  rgSyncToken += ".sync";
  return new RGSystemSyncLock(rgSyncToken);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief constructor.
///////////////////////////////////////////////////////////////////////////////
RGFileLockImpl::RGFileLockImpl(const std::string &name) : m_name(name) {}
