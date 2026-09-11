 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _REFERENCE_KEY_CONTAINER_H
#define _REFERENCE_KEY_CONTAINER_H

#include "ReferenceContainer.h"
#include <qmap.h>

template<class TITEM, class TKEY>
class CReferenceKeyContainer : public CReferenceContainer<TITEM>
{
public:
  CReferenceKeyContainer( const QString& text );

  virtual TKEY getKey( const TITEM& item ) const = 0;

  const TITEM* findKey( TKEY key ) const;

protected:
  // overrides
  virtual void referenceInserted( const IModelObject& origin,
                                  const IModelObject& reference );
  virtual void referenceRemoved(  const IModelObject& origin,
                                  const IModelObject& reference );

private:
  typedef QMap<TKEY,const TITEM*> TKeyItemMap;
  TKeyItemMap m_itemKeys;
};

/*!
  \class CReferenceKeyContainer

  A template container for IModelObject derived items, providing
  access to the items based on a unique key.

  - The TITEM type must be derived from IModelObject.
  - The TKEY type must have an operator< for ordering the keys.
  
  The contained items must have a key which is unique in the container.

  To populate the container, call CReferenceKeyContainer::insertReference()
  for each item, passing this container as parent.

  Destruction of an item or reparenting an item automatically removes
  the item from the container and releases its key.

  Reimplement getKey() to retreive a unique key for an item.

  To retreive contained objects by their key use findKey().
*/

/*!
  Creates a container for TITEM objects, accessible by a key.
  
  \param text User visible name of the container
  \param icon Icon of the container to be used in gui (optional)

  To finalize construction, call create() afterwards.
*/
template<class TITEM, class TKEY>
CReferenceKeyContainer<TITEM,TKEY>::CReferenceKeyContainer(const QString& text)
: CReferenceContainer<TITEM>( text )
{
}

/*!
  Returns the contained TITEM object having the requested key.
  If no such object is found, a zero is returned.
*/
template<class TITEM, class TKEY>
const TITEM* CReferenceKeyContainer<TITEM,TKEY>::findKey( TKEY key ) const
{
  // search item with key, return if found, zero otherwise
  typename TKeyItemMap::const_iterator it = m_itemKeys.find( key );
  if ( it == m_itemKeys.end() ) return 0;
  return *it;
}

/*!
  Called by the model framework when a reference item is inserted.

  The member is overridden to retreive and store the key of the inserted
  item. The key must be unique to the container.
  
  The creator of the item is responsible for uniqueness of the key.
  Uniqueness can be checked beforehand with findKey().

  \sa IModelObject::create, IModelObject::reParent.
*/
template<class TITEM, class TKEY>
void CReferenceKeyContainer<TITEM,TKEY>::referenceInserted(
                     const IModelObject& origin, const IModelObject& reference )
{
  if ( &origin == this ) {
  const TITEM* item = dynamic_cast<const TITEM*>( &reference );
  if ( item ) {
      TKEY key = getKey( *item );
      // store relation between item and key; key must not exist
      typename TKeyItemMap::iterator it = m_itemKeys.find( key );
      if ( it == m_itemKeys.end() )
      {
    it = m_itemKeys.insert( key, item );
      }
      assert( *it == item ); // The key must not belong to another child item
  }
  }
  CReferenceContainer<TITEM>::referenceInserted( origin, reference );
}

/*!
  Called by the model framework when a referenced item is removed.

  The member is overridden to retreive and release the key of the
  removed item.
  
  \sa IModelObject::destroy, IModelObject::reParent.
*/
template<class TITEM, class TKEY>
void CReferenceKeyContainer<TITEM,TKEY>::referenceRemoved(
                     const IModelObject& origin, const IModelObject& reference )
{
  if ( &origin == this ) {
  const TITEM* item = dynamic_cast<const TITEM*>( &reference );
  if ( item ) {
      TKEY key = getKey( *item );
      // forget relation between item and key; key must exist
      typename TKeyItemMap::iterator it = m_itemKeys.find( key );
      assert( it != m_itemKeys.end() );
      assert( *it == item ); // The key must not belong to another child item
      m_itemKeys.erase( it );
  }
  }
  CReferenceContainer<TITEM>::referenceRemoved( origin, reference );
}

#endif // _MODEL_KEY_CONTAINER_H

