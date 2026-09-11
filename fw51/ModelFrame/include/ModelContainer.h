/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include "IModelObject.h"
#include "IModelObjectFactory.h"
#include "IModelStream.h"
#include "IProgressBase.h"
#include "StreamVersion.h"
#include <QRegExp>
#include <cassert>

/**
 *@author
 */
template <class T, class MODEL_OBJECT = IModelObject> class CModelContainer : public MODEL_OBJECT {
public:
  typedef T TItemType;
  typedef IModelObject::TStream TStream;

  CModelContainer();
  CModelContainer(const QString &name);

  int size() const;
  const T &at(int nIndex) const;
  T &at(int nIndex);
  virtual bool empty() const;
};

/*!
  Default construction of the model container. A load action should follow.
  \sa IModelObject::create
*/
template <class T, class MODEL_OBJECT> CModelContainer<T, MODEL_OBJECT>::CModelContainer() : MODEL_OBJECT() {}

/*!
  Construction of the model container.
  \param name is the name of the container
  Construction must be followed by a call to create.
  \sa IModelObject::create
*/
template <class T, class MODEL_OBJECT>
CModelContainer<T, MODEL_OBJECT>::CModelContainer(const QString &name) : MODEL_OBJECT(name) {}

/*!
  Return the amount of objects in the container
*/
template <class T, class MODEL_OBJECT> int CModelContainer<T, MODEL_OBJECT>::size() const {
  return MODEL_OBJECT::childSize();
}

/*!
  Returns the object at a certain position in the container
*/
template <class T, class MODEL_OBJECT> const T &CModelContainer<T, MODEL_OBJECT>::at(int nIndex) const {
  return (const T &)MODEL_OBJECT::childAt(nIndex);
}

/*!
  Returns the object at a certain position in the container
*/
template <class T, class MODEL_OBJECT> T &CModelContainer<T, MODEL_OBJECT>::at(int nIndex) {
  return (T &)MODEL_OBJECT::childAt(nIndex);
}

template <class T, class MODEL_OBJECT> bool CModelContainer<T, MODEL_OBJECT>::empty() const {
  return this->size() == 0;
}

template <typename TContainer> QString suggestUniqueName(const TContainer *container, QString format) {
  if (!format.contains("%1"))
    format += "%1";

  QRegExp re = QRegExp(format.arg("(\\d+)"));

  // First, find the number of the highest numbered item
  int highest = 0;
  for (int i = 0; i != container->size(); ++i) {
    QString name = container->at(i).text();
    if (re.indexIn(name) != -1) {    // match found
      int value = re.cap(1).toInt(); // extract the number
      if (value > highest)
        highest = value;
    }
  }
  // We now (should) have a unique name if we increase highest & generate a name
  highest++;
  return format.arg(highest);
}

#endif // MODELCONTAINER_H
