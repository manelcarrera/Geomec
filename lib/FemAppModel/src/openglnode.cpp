// OpenGLNode.cpp: implementation of the COpenGLNode class.
//
//////////////////////////////////////////////////////////////////////

#include "StorageNode.h"
#include "OpenGLNodeBase.h"
#include "openglnode.h"
#include "IConus.h"

COpenGLNode::CDrawDef::CDrawDef(const COpenGLNode &node)
: m_node(node)
, m_zeroDrawDef()
{
}

const COpenGLNode& COpenGLNode::CDrawDef::OpenGLNode() const
{
	return m_node;
}

COpenGLNode::CDrawDef::DrawDecisionVector
  COpenGLNode::CDrawDef::Color(const geo::IObject &/*object*/) const
{
  std::vector <TColor> empty;

  return std::make_pair(false, empty);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::Visible() const
{
  return std::make_pair(true, true);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::PolyFillFront() const
{
  return std::make_pair(false, false);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::PolyFillBack() const
{
  return std::make_pair(false, false);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::LineStipple() const
{
  return std::make_pair(true, false);
}

COpenGLNode::CDrawDef::DrawDecisionFloat
  COpenGLNode::CDrawDef::LineWidth() const
{
  return std::make_pair(true, 1.0f);
}

COpenGLNode::CDrawDef::DrawDecisionFloat
  COpenGLNode::CDrawDef::PointSize() const
{
  return std::make_pair(false, 0.0f);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::PolyDrawFront() const
{
  return std::make_pair(true, true);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::PolyDrawBack() const
{
  return std::make_pair(true, true);
}

COpenGLNode::CDrawDef::DrawDecisionInt
  COpenGLNode::CDrawDef::DepthFunc() const
{
  return std::make_pair(true, GL_LESS);
}

bool COpenGLNode::Value() const
{
	return TRUE;
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::CurvedDraw() const
{
  return std::make_pair(true, true);
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::IsSelectable(const geo::IObject& object) const
{
	for(int i = 0; i < m_node.DisplayListSize(); i++)
	{
		if(&m_node.DisplayList(i) == &object)
			return std::make_pair(true, true);
	}

	return std::make_pair(true, false);
}

COpenGLNode::CDrawDef::DrawDecisionColor
  COpenGLNode::CDrawDef::TextColor() const
{
  return std::make_pair(true, m_zeroDrawDef.TextColor());
}

COpenGLNode::CDrawDef::DrawDecisionGLint
  COpenGLNode::CDrawDef::LineStippleFactor() const
{
  return std::make_pair(true, m_zeroDrawDef.LineStippleFactor());
}

COpenGLNode::CDrawDef::DrawDecisionGLushort
  COpenGLNode::CDrawDef::LineStipplePattern() const
{
  return std::make_pair(true, m_zeroDrawDef.LineStipplePattern());
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::RequireCompile() const
{
  return std::make_pair(true, m_zeroDrawDef.RequireCompile());
}

COpenGLNode::CDrawDef::DrawDecisionLong
  COpenGLNode::CDrawDef::CircleNrOfSegments() const
{
  return std::make_pair(true, m_zeroDrawDef.CircleNrOfSegments());
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::LabelBorder() const
{
  return std::make_pair(true, m_zeroDrawDef.LabelBorder());
}

COpenGLNode::CDrawDef::DrawDecisionColor
  COpenGLNode::CDrawDef::LabelBorderOutlineColor() const
{
  return std::make_pair(true, m_zeroDrawDef.LabelBorderOutlineColor());
}

COpenGLNode::CDrawDef::DrawDecisionColor
  COpenGLNode::CDrawDef::LabelBorderInteriorColor() const
{
  return std::make_pair(true, m_zeroDrawDef.LabelBorderInteriorColor());
}

COpenGLNode::CDrawDef::DrawDecisionBool
  COpenGLNode::CDrawDef::UseStencilBuffer() const
{
  return std::make_pair(true, m_zeroDrawDef.UseStencilBuffer());
}

COpenGLNode::CDrawDef::DrawDecisionGLenum
  COpenGLNode::CDrawDef::StencilFuncFunc() const
{
  return std::make_pair(true, m_zeroDrawDef.StencilFuncFunc());
}

COpenGLNode::CDrawDef::DrawDecisionGLint
  COpenGLNode::CDrawDef::StencilFuncRef() const
{
  return std::make_pair(true, m_zeroDrawDef.StencilFuncRef());
}

COpenGLNode::CDrawDef::DrawDecisionGLuint
  COpenGLNode::CDrawDef::StencilFuncMask() const
{
  return std::make_pair(true, m_zeroDrawDef.StencilFuncMask());
}

COpenGLNode::CDrawDef::DrawDecisionGLenum
  COpenGLNode::CDrawDef::StencilOpFail() const
{
  return std::make_pair(true, m_zeroDrawDef.StencilOpFail());
}

COpenGLNode::CDrawDef::DrawDecisionGLenum
  COpenGLNode::CDrawDef::StencilOpZFail() const
{
  return std::make_pair(true, m_zeroDrawDef.StencilOpZFail());
}

COpenGLNode::CDrawDef::DrawDecisionGLenum
  COpenGLNode::CDrawDef::StencilOpZPass() const
{
  return std::make_pair(true, m_zeroDrawDef.StencilOpZPass());
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLNode::COpenGLNode(CFemAppModel& model)
: COpenGLNodeBase(model)
{
}

COpenGLNode::COpenGLNode(const QString &strName, CFemAppModel& model)
: COpenGLNodeBase(strName, model)
{
}

COpenGLNode::COpenGLNode(const unsigned int uName, CFemAppModel& model)
: COpenGLNodeBase(uName, model)
{
}

COpenGLNode::COpenGLNode(const COpenGLNode &rhs)
: COpenGLNodeBase(rhs)
{
}

geo::CPoint COpenGLNode::Min() const
{
	geo::CPoint ret;
	for(int i = 0; i < DisplayListSize(); i++)
		ret = ret.Min(DisplayList(i).Min());

	return ret;
}

geo::CPoint COpenGLNode::Max() const
{
	geo::CPoint ret;
	for(int i = 0; i < DisplayListSize(); i++)
		ret = ret.Max(DisplayList(i).Max());

	return ret;
}

COpenGLNode::CDrawDef* COpenGLNode::OnCreateDrawDef(const geo::IObject& /*object*/) const
{
	return new CDrawDef(*this);
}

std::vector<COpenGLNode::CDrawDef::TColor> COpenGLNode::OnColor(const geo::IObject &object) const
{
	const geo::IElement *pElement = dynamic_cast<const geo::IElement*>(&object);
	const geo::ILine *pLine		  = dynamic_cast<const geo::ILine*>(&object);
	const geo::IPoint *pPoint	  = dynamic_cast<const geo::IPoint*>(&object);
	const geo::IConus *pConus = dynamic_cast<const geo::IConus *>(&object);

	std::vector<CDrawDef::TColor> vcColor;
	if(pElement)
	{
		for(int i = 0; i < pElement->NrOfPoints(); i++)
		{
			vcColor.push_back(Color());
		}
	}
	else if(pLine)
	{
		vcColor.push_back(Color());
		vcColor.push_back(Color());
	}
	else if( pConus) //wjrx mantis 3185
	{
		vcColor.push_back(Color());
	}
	else if(pPoint)
	{
		vcColor.push_back(Color());
	}
	else
	{
		assert(false);
	}

	return vcColor;
}

COpenGLNode& COpenGLNode::operator=(const COpenGLNode &rhs)
{
	COpenGLNodeBase::operator=(rhs);
	return *this;
}

bool COpenGLNode::operator==(const COpenGLNode &rhs) const
{
	return COpenGLNodeBase::operator==(rhs);
}

int COpenGLNode::SelectionPriority() const
{
	// default priority is 0, use higher or lower priority in derived
  // classes to tune selection priority in the scene
	return 0;
}
