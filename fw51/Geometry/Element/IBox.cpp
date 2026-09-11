/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IBox.cpp: implementation of the IBox class.
//
//////////////////////////////////////////////////////////////////////
#include "IBox.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

IBox::IBox() {}

IBox::IBox(const IBox & /*box*/) {}

IBox::~IBox() {}

double IBox::Size() const { return Width() * Depth() * Height(); }

} // namespace geo
