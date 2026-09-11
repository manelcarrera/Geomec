
#include "Delegate.h"

/*!
 * \class CDelegate
 * Base class for all delegate types.
 * Implements the (template) Create method that is used by CDelegateFactory
 * Holds (template) CDelegateTypeInfo. A derived class of this type must be
 * declared in each derived delegate type so it will be registered in the
 * factory. Use the REGISTER_DELEGATE macro for this.
 */

CDelegate::~CDelegate() {}

CDelegate::CDelegate() {}
