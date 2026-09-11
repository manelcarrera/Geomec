 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "Operation.h"
#include "DocumentBase.h"
#include "ModelOperation.h"

/*!
  \class COperation
  \brief obsolete, to be replaced with CModelOperation
*/

COperation::COperation( IModelObject& object )
: m_document( object.document() )
{
  m_temp = new CModelOperation( object, 0 );
}

COperation::COperation( IModelObject& object, TOperationType /*type*/, const QString& /*text*/, int /*customID*/ )
: m_document( object.document() )
{
  m_temp = new CModelOperation( object, 0 );
}

COperation::~COperation()
{
  delete m_temp;
}

void COperation::undo()
{
}

void COperation::onDocumentDestroyed()
{
  m_document = 0;
}
