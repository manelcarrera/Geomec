// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGLock.h"

RGLock::RGLock(RGSync::ProcessRole role, const std::string &modelName)
    : m_role(role), m_modelName(new std::string(modelName)) {}

RGLock::~RGLock() { delete m_modelName; }

RGSync::ProcessRole RGLock::role() const { return m_role; }

const std::string RGLock::model() const { return *m_modelName; }
