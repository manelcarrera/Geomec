/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ISelectionContext.h"
#include <cassert>

/*!
  \class ISelectionContext
  \todo getState() redesign.
  \todo full doxy
*/

/*!
  Constructs a selection context for the owner object.
*/
ISelectionContext::ISelectionContext(QObject *owner) : QObject(owner) { setObjectName("ISelectionContext"); }

/*!
  Destroys the object and releases any allocated resources.
*/
ISelectionContext::~ISelectionContext() {}

void ISelectionContext::deleteSelection() { assert(false); }

bool ISelectionContext::canDeleteSelection() const { return false; }

void ISelectionContext::setDirty() { emit selectionChanged(); }
