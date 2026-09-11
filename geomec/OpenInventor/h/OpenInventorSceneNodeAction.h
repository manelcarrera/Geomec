#ifndef _OpenInventorSceneNodeAction_h_
#define _OpenInventorSceneNodeAction_h_

class COpenInventorSceneNode;

#include "BusinessAction.h"

// The include is mandatory for the member COpenInventorSceneNode::*m_func_ptr.

#include "OpenInventorSceneNode.h"

// Class to define actions to be taken upon creation of a list of points by the event handler

class COpenInventorSceneNodeAction : public CWorldAction {
public:
  COpenInventorSceneNodeAction(COpenInventorSceneNode *receiver,
                               void (COpenInventorSceneNode::*func_ptr)(void *user_data,
                                                                        const std::vector<geo::CPoint> &),
                               void *user_data);

  void operator()(const std::vector<geo::CPoint> &points);

private:
  COpenInventorSceneNode *m_receiver;
  void (COpenInventorSceneNode::*m_func_ptr)(void *user_data, const std::vector<geo::CPoint> &);
  void *m_userData;
};

#endif // _OpenInventorSceneNodeAction_h_
