 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __I_MODEL_OBJECT_H
#define __I_MODEL_OBJECT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <vector>
#include "Operation.h"

#include "ModelFrameExports.h"

class IModelStream;
class IProgressBase;
class CStreamVersion;
class IModelObjectVisitor;
class CDocumentBase;
class CUndoOperation;
namespace geo { class IObject; }

class MODELFRAME_EXPORT  IModelObject : public QObject
{
  Q_OBJECT
public:
  typedef IModelStream TStream;
private:
  // QPtrList instead of std::vector for performance reasons, iterating
  // is fast enough and lookup, insertion and removal are faster
  //
  // QT4_REVIEW have not benchmarked this for QList on Qt4
  QList<IModelObject*> m_children;		    // Children of the model object
  QList<const IModelObject*> m_references;	// References of the model object
  mutable QList<IModelObject*> m_links;	// Links to model objects referencing this
  QRgb m_color;
  QString m_text;
  bool m_created;
  bool m_destroyed;
protected:
  virtual void onCreate();
  virtual void onDestroy();

  void stateChanged();
  virtual void stateChanged( IModelObject& origin );
  virtual void refStateChanged( IModelObject& ref );
  void displayChanged();
  virtual void displayChanged( IModelObject& origin );
  virtual void refDisplayChanged( IModelObject& ref );
  void propertyChanged();
  virtual void propertyChanged( IModelObject& origin );
  virtual void refPropertyChanged( IModelObject& ref );
  void geometryChanged();
  virtual void geometryChanged( IModelObject& origin );
  virtual void refGeometryChanged( IModelObject& ref );
  void parameterChanged();
  virtual void parameterChanged( IModelObject& origin );
  virtual void refParameterChanged( IModelObject& ref );
  void childInserted( IModelObject& child );
  virtual void childInserted( IModelObject& origin, IModelObject& child );
  virtual void refChildInserted( IModelObject& ref, IModelObject& child );
  void childRemoved( IModelObject& child );
  virtual void childRemoved( IModelObject& origin, IModelObject& child );
  virtual void refChildRemoved( IModelObject& ref, IModelObject& child );
  void referenceInserted( const IModelObject& ref );
  virtual void referenceInserted( const IModelObject& origin, const IModelObject& ref);
  void referenceRemoved( const IModelObject& ref );
  virtual void referenceRemoved( const IModelObject& origin, const IModelObject& ref);
  void error( const QString& description );
  virtual void error( IModelObject& origin, const QString& description );
  void warning( const QString& description );
  virtual void warning( IModelObject& origin, const QString& description );

  typedef QListIterator<IModelObject*> TModelObjectIterator;
  TModelObjectIterator referenceOrigins() const;

signals:
  void onStateChanged();		//*!< Emitted when the state of the object is changed.
  void onDisplayChanged(IModelObject&);	//*!< Emitted when the display properties of an object are changed.
  void onPropertyChanged( IModelObject& ); //*!< A property of the object is modified
  void onGeometryChanged();	//*!< The geometry of a the object is changed.
  void onParameterChanged();	//*!< A model parameter is changed and the results.
  void onChildOrderChanged();	//*!< The order of child nodes changed
  void onChildInserted(IModelObject&, IModelObject&);		//*!< A child is inserted.
  void onChildRemoved(IModelObject&, IModelObject&);		//*!< A child is removed
  void onReferenceInserted();	//*!< A reference is inserted.
  void onReferenceRemoved();	//*!< A reference is removed
  void onDestroyed();			//*!< The object is destroyed. Only the IModelObject class
  						// is accessible.
  void onError( const QString& description ); //*!< An error occurred
  void onWarning( const QString& description ); //*!< A warning occurred
public:
  typedef std::vector<IModelObject*> TModelObjectVec;
  virtual ~IModelObject();

  bool isCreated() const { return m_created; }
  void create( IModelObject*       parent = 0,
               const IModelObject* pInsertBefore = 0 );
  void reParent( IModelObject*       new_parent,
                 const IModelObject* pInsertBefore = 0);
  virtual void insertChild(QObject *pObject);
  virtual void removeChild(QObject *pObject);
  int childSize() const;
  const IModelObject& childAt(int nIndex) const;
  IModelObject& childAt(int nIndex);
  const IModelObject* findChild( const QString& text ) const;
  IModelObject* findChild( const QString& text );
  int isChild(const IModelObject& object) const;
  bool isChildInTree(const IModelObject& object) const;

  void moveChildren( const QList<IModelObject*>& children,
                     int destinationIndex );
  void sortChildren();

  virtual void setText(const QString& new_text);
  virtual QString text() const;

  const CDocumentBase* document() const;
  CDocumentBase* document();

  int index() const;

  // Reference interface
  int referenceSize() const;
  const IModelObject& referenceAt(int nIndex) const;
  int insertReference( const IModelObject& object, const IModelObject* insertBefore = 0 );
  int isReferenced(const IModelObject& object) const;
  void removeReference(int nIndex);
  void removeReference( const IModelObject& object );

  // Geometry export and color
  QRgb color() const;
  void color(QRgb c);
  virtual int geometrySize() const;
  virtual const geo::IObject& geometry(int nIndex) const;

  // Visitor
  bool acceptVisitor(IModelObjectVisitor& visitor);

  // Returns true when modelobject does not contain information
  virtual bool empty() const;
  virtual bool valid() const;

  //derived objects can give reason for invalid state
  virtual QString getInvalidReasonDescription() const;

  void destroy();
  virtual bool canDestroy() const;

  virtual void Connect(const TModelObjectVec& object);
  virtual bool CanConnect(const TModelObjectVec& object);

  virtual void clear();

  // The store and restore functions are used for saving, loading, redo and undo
  virtual void restore( TStream& stream,
            const CStreamVersion& file_version,
            IProgressBase& indicator );
  virtual void store( TStream& stream,
                      IProgressBase& indicator,
                      bool includeChildren = true ) const;
  virtual int storeSteps( bool includeChildren = true ) const;
  virtual void storeReferences( TStream& stream ) const;
  virtual void restoreReferences( TStream& stream,
                                  const CStreamVersion& fileVersion );

  virtual bool less(const IModelObject& rhs) const;

protected: // abstract class: prevent instantiation
  IModelObject();
  IModelObject( const QString& text );

  virtual CUndoOperation* constructCreateUndoOperation( IModelObject& object,
                            const QString& descr );
  virtual CUndoOperation* constructDestroyUndoOperation( IModelObject& location,
                                                          IModelObject& object,
                                                          const QString& descr);

private:
  void addToParent( IModelObject*       parent,
          const IModelObject* pInsertBefore = 0);

private: // not copyable
  IModelObject( const IModelObject& );
  IModelObject& operator=( const IModelObject& );
};

#endif //  __I_MODEL_OBJECT_H
