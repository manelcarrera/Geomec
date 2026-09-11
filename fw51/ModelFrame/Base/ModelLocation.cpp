/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ModelLocation.h"
#include "DocumentBase.h"
#include "IModelStream.h"
#include <cassert>

/*!
  \class CModelLocation
  \brief Stores/restores the location of a CDocumentBase child.

  The position of an IModelObject in a CDocumentBase is represented for
  persistency purposes.

  The class can be used when a pointer is not usable for reverting state
  changes and for storage and restore operations.
*/

/*!
  Constructs a default CModelLocation object to be restored from a buffer.
  \sa restore().
*/
CModelLocation::CModelLocation() {}

/*!
  Constructs a CModelLocation object, representing the document location of
  object.
*/
CModelLocation::CModelLocation(const IModelObject &object) {
  const IModelObject *child = &object;
  const IModelObject *parent = dynamic_cast<const IModelObject *>(child->parent());
  while (parent) {
    int index = -1;
    for (int i = 0; i < parent->childSize(); ++i)
      if (&parent->childAt(i) == child) {
        index = i;
        break;
      }
    assert(index != -1);

    m_positions.push(index);
    child = parent;
    parent = dynamic_cast<const IModelObject *>(child->parent());
  }
  assert(dynamic_cast<const CDocumentBase *>(child));
}

/*!
  Destroys the object and frees any allocated resources.
*/
CModelLocation::~CModelLocation() {}

/*!
  Returns the (grant)child object of document at the location being
  represented by this object.
*/
IModelObject *CModelLocation::getObject(CDocumentBase &document) {
  IModelObject *object = &document;
  QStack<int> positions = m_positions;
  while (positions.size()) {
    int index = positions.pop();

    object = &object->childAt(index);
  }
  return object;
}

/*!
  Stores the location in a buffer.
*/
void CModelLocation::store(TStream &stream) const {
  stream << (int)m_positions.size();
  QStack<int>::const_iterator it;
  for (it = m_positions.begin(); it != m_positions.end(); ++it) {
    stream << *it;
  }
}

/*!
  Restores the location from a buffer.
*/
void CModelLocation::restore(TStream &stream, const CStreamVersion & /*streamVersion*/) {
  int positionSize;
  stream >> positionSize;
  for (int i = 0; i < positionSize; ++i) {
    int pos;
    stream >> pos;
    m_positions.push(pos);
  }
}
