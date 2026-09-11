 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _MODEL_KEY_CONTAINER_H
#define _MODEL_KEY_CONTAINER_H

#include "ModelContainer.h"
#include <QMap>
#include <QList>
#include <QRegExp>

#include "IModelStream.h"
#include "StreamVersion.h"
#include "IProgressBase.h"
#include "DocumentBase.h"
#include "ModelOperation.h"
#include "SimpleUndo.h"
#include "ModelLocation.h"

template<class TITEM, class TKEY>
class CModelKeyContainer : public CModelContainer<TITEM>
{
public:
  CModelKeyContainer( const QString& text );

  virtual TKEY getKey( const TITEM& item ) const = 0;

  const TITEM* findKey( TKEY key ) const;
  TITEM* findKey( TKEY key );

  QList< TKEY > keys() const;

  virtual QString suggestUniqueName( QString format = "item %1" ) const ;

protected:
  // overrides
  virtual void childInserted( IModelObject& origin, IModelObject& child );
  virtual void childRemoved( IModelObject& origin, IModelObject& child );

  typedef IModelObject::TStream TStream;
	virtual void restore(TStream& stream, const CStreamVersion& file_version, IProgressBase& indicator);
	virtual void store(TStream& stream, IProgressBase& indicator, bool includeChildren = true ) const;
	virtual TKEY restoreKey(TStream& stream, const CStreamVersion& file_version, IProgressBase& indicator) = 0;
  virtual void storeKey(TKEY key, TStream& stream, IProgressBase& indicator) const = 0;

  virtual QString nameOf( const TITEM* item ) const;
  bool    isUniqueName( const QString& name ) const;

private:
  typedef QMap<TKEY,TITEM*> TKeyItemMap;
  TKeyItemMap m_itemKeys;
};

/*!
  \class CModelKeyContainer

	A template container for IModelObject derived items, providing
	access to the items based on a unique key.

	- The TITEM type must be derived from IModelObject.
	- The TKEY type must have an operator< for ordering the keys.
	
	The contained items must have a key which is unique in the container.

	To populate the container, call IModelObject::create() or
	IModelObject::reParent() for each item, passing this container as
	parent.

	Destruction of an item or reparenting an item automatically removes
	the item from the container and releases its key.

	Reimplement getKey() to retreive a unique key for an item.

	To retreive contained objects by their key use findKey().

  An additional service provided by the CModelKeyContainer class is unique
  name suggestions. This service is primarily useful in cases where the
  container needs to guarantee a TITEM field is a unique string (e.g. the name).
  Before inserting into the container, clients of the container class can obtain
  a suggestion that is guaranteed to be unique with respect to the TITEMS that
  are already in the container.

  To retreive a unique name suggestion, use the suggestUniqueName() method.

  Derived classes that need to provide this functionality must override the
  nameOf() method.
*/

/*!
	Creates a container for TITEM objects, accessible by a key.
	
	\param text User visible name of the container
	\param icon Icon of the container to be used in gui (optional)

	To finalize construction, call create() afterwards.
*/
template<class TITEM, class TKEY>
CModelKeyContainer<TITEM,TKEY>::CModelKeyContainer( const QString& text )
: CModelContainer<TITEM>( text )
{
}

/*!
	Returns the contained TITEM object having the requested key.
	If no such object is found, a zero is returned.
*/
template<class TITEM, class TKEY>
const TITEM* CModelKeyContainer<TITEM,TKEY>::findKey( TKEY key ) const
{
  // search item with key, return if found, zero otherwise
  typename TKeyItemMap::const_iterator it = m_itemKeys.find( key );
  if ( it == m_itemKeys.end() ) return 0;
  return it.value();
}

/*!
	Returns the contained TITEM object having the requested key.
	If no such object is found, a zero is returned.
*/
template<class TITEM, class TKEY>
TITEM* CModelKeyContainer<TITEM,TKEY>::findKey( TKEY key )
{
  // search item with key, return if found, zero otherwise
  typename TKeyItemMap::iterator it = m_itemKeys.find( key );
  if ( it == m_itemKeys.end() ) return 0;
  return it.value();
}

/*!
  Returns a list with all keys in the container
*/
template<class TITEM, class TKEY>
  QList<TKEY> CModelKeyContainer<TITEM,TKEY>::keys() const
{
  return m_itemKeys.keys();
}

/*!
	Called by the model framework when a child item is inserted.

	The member is overridden to retreive and store the key of the inserted
	item. The key must be unique to the container.
	
	The creator of the item is responsible for uniqueness of the key.
	Uniqueness can be checked beforehand with findKey().

	\sa IModelObject::create, IModelObject::reParent.
*/
template<class TITEM, class TKEY>
void CModelKeyContainer<TITEM,TKEY>::childInserted( IModelObject& origin, IModelObject& child )
{
  if ( &origin == this ) {
    TITEM* item = dynamic_cast<TITEM*>( &child );
    if ( item ) {
      CModelOperation operation( *this, 0 );
      if ( operation.stackEnabled() )
        operation.push( new CSimpleUndo( *this, 0 ) );
      TKEY key = getKey( *item );
      // store relation between item and key; key must not exist
      typename TKeyItemMap::iterator it = m_itemKeys.find( key );
      if ( it == m_itemKeys.end() ) {
        it = m_itemKeys.insert( key, item );
      }
      assert( it.value() == item ); // The key must not belong to another child item
    }
  }
  CModelContainer<TITEM>::childInserted( origin, child );
}

/*!
	Called by the model framework when a child item is removed.

	The member is overridden to retreive and release the key of the
	removed item.
	
	\sa IModelObject::destroy, IModelObject::reParent.
*/
template<class TITEM, class TKEY>
void CModelKeyContainer<TITEM,TKEY>::childRemoved( IModelObject& origin, IModelObject& child )
{
  if ( &origin == this ) {
    TITEM* item = dynamic_cast<TITEM*>( &child );
    if ( item ) {
      CModelOperation operation( *this, 0 );
      if ( operation.stackEnabled() )
        operation.push( new CSimpleUndo( *this, 0 ) );
      TKEY key = getKey( *item );
      // forget relation between item and key
      // key does not need to exist anymore because of ::restore
      typename TKeyItemMap::iterator it = m_itemKeys.find( key );
      if ( it != m_itemKeys.end() ) {
        assert( it.value() == item ); // The key must not belong to another child item
        m_itemKeys.erase( it );
      }
    }
  }
  CModelContainer<TITEM>::childRemoved( origin, child );
}

template<class TITEM, class TKEY>
void CModelKeyContainer<TITEM,TKEY>::restore(TStream& stream, const CStreamVersion& file_version, IProgressBase& indicator)
{
  CModelContainer<TITEM>::restore( stream, file_version, indicator );

  CDocumentBase* doc = stream.getModel()->document();
  assert( doc );
	// Local version for local changes
	CStreamVersion version;
	version.restore(stream);

  m_itemKeys.clear();
  int size;
  stream >> size;
  for ( int i = 0; i < size; ++i ) {
    TKEY key = restoreKey( stream, file_version, indicator );
    CModelLocation position;
    position.restore( stream, file_version );
    IModelObject* object = position.getObject( *doc );
    TITEM* item = dynamic_cast<TITEM*>( object );
    assert( item );
    assert( key == getKey( *item ) );
    // store relation between item and key; key must not exist
    typename TKeyItemMap::iterator it = m_itemKeys.find( key );
    if ( it == m_itemKeys.end() ) {
      it = m_itemKeys.insert( key, item );
    }
    assert( it.value() == item ); // The key must not belong to another child item
  }
}

template<class TITEM, class TKEY>
void CModelKeyContainer<TITEM,TKEY>::store(TStream& stream, IProgressBase& indicator, bool includeChildren ) const
{
  CModelContainer<TITEM>::store( stream, indicator, includeChildren );

  // Local version for local changes
  CStreamVersion version(0, 0, 1);
  version.store(stream);

  stream << (int)m_itemKeys.size();
  typename TKeyItemMap::const_iterator it;
  for ( it = m_itemKeys.begin(); it != m_itemKeys.end(); ++it ) {
    storeKey( it.key(), stream, indicator );
    CModelLocation position( *it.value() );
    position.store( stream );
  }
}

template<class TITEM, class TKEY>
QString CModelKeyContainer<TITEM, TKEY>::suggestUniqueName( QString format ) const
{
  if ( !format.contains( "%1" ) )
    format += "%1";

  QRegExp re = QRegExp( format.arg( "(\\d+)" ) );

  // First, find the number of the highest numbered item
  int highest = 0;
  typename TKeyItemMap::const_iterator it;
  for ( it = m_itemKeys.begin(); it != m_itemKeys.end(); ++it ) {
    QString name = nameOf( it.value() );
    if ( re.indexIn( name ) != -1 ) { // match found
      int value = re.cap( 1 ).toInt();// extract the number
      if ( value > highest ) highest = value;
    }
  }
  // We now (should) have a unique name if we increase highest & generate a name
  QString suggestion = "";
  do {
    highest++;
    suggestion = format.arg( highest );
  } while ( !isUniqueName( suggestion ) );

  return suggestion;
}

template<class TITEM, class TKEY>
bool CModelKeyContainer<TITEM, TKEY>::isUniqueName( const QString& name ) const
{
  typename TKeyItemMap::const_iterator it;
  for ( it = m_itemKeys.begin(); it != m_itemKeys.end(); ++it ) {
    if ( name == nameOf( it.value() ) )
      return false;
  }
  return true;
}

template<class TITEM, class TKEY>
QString CModelKeyContainer<TITEM, TKEY>::nameOf( const TITEM* item ) const
{
  return "";
}

#endif // _MODEL_KEY_CONTAINER_H

