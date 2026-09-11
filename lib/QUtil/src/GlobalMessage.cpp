#include "GlobalMessage.h"

#include "Global.h"

#include "util_.h"

#include <thread>

namespace
{
void init_( IGlobalMessage*& instance_, IGlobalMessage* val )
{
  if(val)
  {
    if( instance_ )
    {
      if( val->type() != instance_->type() )
        delete instance_;
    }
    instance_ = val;
  }
}
void cleanup_(IGlobalMessage*& instance_)
{
  if( instance_ )
  {
    delete instance_;
    instance_ = nullptr;
  }
}
} //namespace


IGlobalMessage* GlobalMessage::m_instance = nullptr;
IGlobalMessage* GlobalMessage::m_instance_mfc = nullptr;

//
// FIXME
//
void GlobalMessage::init_mfc( IGlobalMessage* val )
{
  init_(m_instance_mfc,val);
}

void GlobalMessage::init( IGlobalMessage* val )
{
  init_(m_instance,val);
}


//
// FIXME
//
void GlobalMessage::cleanup()
{
  cleanup_(m_instance);
  cleanup_(m_instance_mfc);
}

IGlobalMessage* GlobalMessage::instance()
{
  IGlobalMessage* instance = _g->is_gui_thread() ? m_instance_mfc : m_instance;

  if( !instance )
    instance = new GlobalMessage_Empty();
  
  return instance;
}

