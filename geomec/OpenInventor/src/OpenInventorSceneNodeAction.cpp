#include "stdafx.h"

#include "OpenInventorSceneNodeAction.h"

COpenInventorSceneNodeAction::COpenInventorSceneNodeAction (COpenInventorSceneNode * receiver, void (COpenInventorSceneNode::*func_ptr)(void * user_data, const std::vector<geo::CPoint> &), void * user_data) :
  m_receiver(receiver), m_func_ptr(func_ptr), m_userData(user_data)
{
}

void COpenInventorSceneNodeAction::operator()(const std::vector<geo::CPoint> & points)
{
  if (m_receiver && m_func_ptr)
  {
    (m_receiver->*m_func_ptr)(m_userData, points);
  }
}
