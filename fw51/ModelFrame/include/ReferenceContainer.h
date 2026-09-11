 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef REFERENCECONTAINER_H
#define REFERENCECONTAINER_H

#include <cassert>
#include "IModelObject.h"

/**
  *@author 
  */
template<class T, class MODEL_OBJECT = IModelObject>
class CReferenceContainer : public MODEL_OBJECT
{
public:
  CReferenceContainer();
  CReferenceContainer(const QString& name );

  int size() const;
  const T& at(int nIndex) const;
};

/*!
  Default construction of the model container. A load action should follow.
  \sa IModelObject::create
*/
template<class T, class MODEL_OBJECT>
CReferenceContainer<T ,  MODEL_OBJECT>::CReferenceContainer() 
: MODEL_OBJECT() 
{
}

/*!
  Construction of the model container.
  \param name is the name of the container
  Construction must be followed by a call to create.
  \sa IModelObject::create
*/
template<class T, class MODEL_OBJECT>
CReferenceContainer<T ,  MODEL_OBJECT>::CReferenceContainer(const QString& name)
: MODEL_OBJECT( name )
{
}

/*!
  Return the amount of objects in the container
*/
template<class T, class MODEL_OBJECT>
int CReferenceContainer<T ,  MODEL_OBJECT>::size() const
{ 
  return MODEL_OBJECT::referenceSize(); 
}

/*!
  Returns the object at a certain position in the container
*/
template<class T, class MODEL_OBJECT>
const T& CReferenceContainer<T ,  MODEL_OBJECT>::at(int nIndex) const 
{ 
  return (const T&)MODEL_OBJECT::referenceAt(nIndex); 
}

#endif
