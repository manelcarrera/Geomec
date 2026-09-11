 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MODEL_OBJECT_OBSERVER_H
#define __MODEL_OBJECT_OBSERVER_H

#include <QObject>
#include <qstringlist.h>
#include "GuiCommand.h"

#include "GuiFrameExports.h"

class IModelObject;
class ICommandInvoker;
class GUIFRAME_EXPORT CModelObjectObserver : public QObject  
{
  Q_OBJECT
  bool m_stateChanged;
  bool m_displayChanged;
  bool m_propertyChanged; 
  bool m_geometryChanged;
  bool m_parameterChanged;
  bool m_childOrderChanged;
  bool m_childInserted;	
  bool m_childRemoved;
  bool m_referenceInserted;
  bool m_referenceRemoved;
  QStringList m_errors;
  QStringList m_warnings;
  IModelObject& m_observed_object;

  bool m_select;
public:
  CModelObjectObserver(IModelObject& observed_object, CModelObjectObserver* parent = 0);
  virtual ~CModelObjectObserver();

  IModelObject& observedObject() { return m_observed_object; }
  const IModelObject& observedObject() const { return m_observed_object; }

  //recursive walk over childs to see if this object is observed, if so select it
  bool selectObject(IModelObject& object);
  bool deSelectObject(IModelObject& object);

  //select or deSelect all childeren
  void selectChildren(CModelObjectObserver* exept=0, bool grand_children =true);
  void deSelectChildren(CModelObjectObserver* exept=0,bool grand_children =true);

  //recursive
  bool isChild(const CModelObjectObserver& observer,bool in_whole_tree=true) const;

  CModelObjectObserver& Root() const;

  // Selection handling
  virtual void select();
  virtual void deSelect();
  virtual bool isSelected() const;

  // Command and menu handling
  virtual void appendCommands( ICommandInvoker& mnu );
  virtual void destroy();
  virtual bool canDestroy() const;
  virtual bool showProperties();
  virtual bool canShowProperties() const;

  //look-up in ALL childeren (recursive)
  CModelObjectObserver*	getObserver(IModelObject& object);

protected:
  // Overrides of stateChanges
  virtual void onStateChanged();		//*!< Emitted when the state of the object is changed.
  virtual void onDisplayChanged();	//*!< Emitted when the display properties of an object are changed.
  virtual void onPropertyChanged();	//*!< The a property of the object is changed.
  virtual void onGeometryChanged();	//*!< The geometry of a the object is changed.
  virtual void onParameterChanged();	//*!< A model parameter is changed and the results.
  virtual void onChildOrderChanged();	//*!< The order of child nodes changed
  virtual void onChildInserted();		//*!< A child is inserted.
  virtual void onChildRemoved();		//*!< A child is removed
  virtual void onReferenceInserted();	//*!< A reference is inserted.
  virtual void onReferenceRemoved();	//*!< A reference is removed
  virtual void onDestroyed();			//*!< Transmitting object destroyed.
  virtual void onError( const QStringList& descriptions );	//*!< Error(s) occurred
  virtual void onWarning( const QStringList& descriptions );	//*!< Warning(s) occurred

  // Overrides of the creation of children
  virtual CModelObjectObserver* insertChild(IModelObject& object);
  virtual int childSize() const;
  virtual IModelObject& childAt(int nIndex);
  virtual void updateChildren();


private slots:
  void applyChangeSignals();	//*!< Function to apply the change signals on the object
  void stateChanged();		//*!< Emitted when the state of the object is changed.
  void displayChanged();		//*!< Emitted when the display properties of an object are changed.
  void propertyChanged();		//*!< The a property of the object is changed.
  void geometryChanged();		//*!< The geometry of a the object is changed.
  void parameterChanged();	//*!< A model parameter is changed and the results.
  void childOrderChanged();	//*!< The order of child nodes changed
  void childInserted();		//*!< A child is inserted.
  void childRemoved();		//*!< A child is removed
  void referenceInserted();	//*!< A reference is inserted.
  void referenceRemoved();	//*!< A reference is removed
  void object_destroyed();	//*!< Transmitting object destroyed
  void error( const QString& description );	//*!< An error occurred
  void warning( const QString& description );	//*!< A warning occurred
};

typedef CGuiCommand<CModelObjectObserver> TModelObjectObserverCommand;

#endif // __MODEL_OBJECT_OBSERVER_H
