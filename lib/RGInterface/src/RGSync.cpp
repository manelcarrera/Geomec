// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGSync.h"

#include "RGFileLock.h"

RGLock *createLock(RGSync::ProcessRole role, const std::string &modelName) { return new RGFileLock(role, modelName); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Constructor
///
/// @param role      the role of the caller master/slave
/// @param modelName the name of the locks.
///////////////////////////////////////////////////////////////////////////////
RGSync::RGSync(RGSync::ProcessRole role, const std::string &modelName) { m_lock = createLock(role, modelName); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Destructor
///////////////////////////////////////////////////////////////////////////////
RGSync::~RGSync(void) { delete m_lock; }

void RGSync::lock() { m_lock->lock(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize the locking mechanism.
///////////////////////////////////////////////////////////////////////////////
void RGSync::init() { m_lock->init(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief release all the resources so the other process can continue.
///////////////////////////////////////////////////////////////////////////////
void RGSync::release() { m_lock->release(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief wait for other process.
/// @param timeout number of seconds the method will wait.
/// @return true if lock is given free. false if the timeout has expired.
///////////////////////////////////////////////////////////////////////////////
bool RGSync::waitFor(int timeout) { return m_lock->waitFor(timeout); }
